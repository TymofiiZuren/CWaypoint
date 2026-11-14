#include "study_store.hpp"
#include "lab.hpp"
#include <cerrno>
#include <fcntl.h>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <unistd.h>
namespace cw {namespace {
bool validPreferences(const Preferences& p){
 if(p.name.size()>80 || p.goal<1 || p.goal>3)return false;
 if(p.editorSize<12 || p.editorSize>24 || p.editorSize%2)return false;
 for(unsigned char c:p.name)if(c<32 || c>126)return false;
 return true;
}
bool validEntry(const std::string& id,const StudyEntry& e){
 return !id.empty() && id.size()<=128 && e.note.size()<=65536 && e.source.size()<=65536 && e.cursor<=e.source.size();
}
bool writeBytes(int fd,const std::string& bytes){
 size_t offset=0;while(offset<bytes.size()){
  auto n=write(fd,bytes.data()+offset,bytes.size()-offset);
  if(n<0 && errno==EINTR)continue;
  if(n<=0)return false;
  offset+=static_cast<size_t>(n);
 }return fsync(fd)==0;
}
bool syncDirectory(const std::filesystem::path& path){int dir=open(path.c_str(),O_RDONLY);if(dir<0)return false;bool ok=fsync(dir)==0;close(dir);return ok;}
bool publish(const std::filesystem::path& path,const std::string& bytes){
 std::string tmp=path.string()+".tmp-XXXXXX";int fd=mkstemp(tmp.data());if(fd<0)return false;
 bool ok=writeBytes(fd,bytes);if(close(fd))ok=false;
 if(ok)ok=rename(tmp.c_str(),path.c_str())==0;
 if(!ok){unlink(tmp.c_str());return false;}
 return syncDirectory(path.parent_path());
}
bool readEntries(std::istream& in,std::map<std::string,StudyEntry>& entries,size_t count){
 for(size_t i=0;i<count;++i){
  std::string id;StudyEntry e;
  if(!(in>>std::quoted(id)>>std::quoted(e.note)>>std::quoted(e.source)>>e.bookmark>>e.hasSource>>e.cursor))return false;
  if(!validEntry(id,e) || entries.count(id))return false;
  entries[id]=std::move(e);
 }return true;
}
}
StudyStore::StudyStore(Store& s):owner(s),generation(s.activeGeneration()){
 supported=load();if(!supported)notice="Study data is damaged or from a newer version. Files preserved; changes cannot be saved.";
}
bool StudyStore::load(){
 const auto path=owner.directory()/"study-v1.save";std::error_code ec;
 if(!std::filesystem::exists(path,ec))return !ec;
 auto size=std::filesystem::file_size(path,ec);if(ec || size>2*1024*1024)return false;
 std::ifstream in(path);std::string magic,end,extra;int version;unsigned long gen;size_t count;Preferences p;
 if(!(in>>magic>>version>>gen) || magic!="CWSTUDY" || version!=1)return false;
 if(!(in>>std::quoted(p.name)>>p.goal>>p.editorSize>>p.lightEditor>>p.nudge>>count) || count>100)return false;
 if(!validPreferences(p) || !readEntries(in,entries,count))return false;
 if(!(in>>end) || end!="END" || (in>>extra))return false;
 preferences=p;if(gen!=generation)entries.clear();notice="Saved on this Mac";return true;
}
bool StudyStore::syncGeneration(){
 if(generation==owner.activeGeneration())return false;
 generation=owner.activeGeneration();entries.clear();unsaved=true;return true;
}
bool StudyStore::save(){
 unsaved=true;
 if(!writable() || !validPreferences(preferences) || entries.size()>100){notice="Study changes cannot be saved. Check preferences and profile access.";return false;}
 std::ostringstream out;out<<"CWSTUDY 1 "<<generation<<'\n'<<std::quoted(preferences.name)<<' '<<preferences.goal<<' '<<preferences.editorSize<<' '<<preferences.lightEditor<<' '<<preferences.nudge<<' '<<entries.size()<<'\n';
 for(const auto& [id,e]:entries){if(!validEntry(id,e)){notice="Note or source exceeds its limit.";return false;}out<<std::quoted(id)<<' '<<std::quoted(e.note)<<' '<<std::quoted(e.source)<<' '<<e.bookmark<<' '<<e.hasSource<<' '<<e.cursor<<'\n';}
 out<<"END\n";
 if(out.str().size()>2*1024*1024 || !publish(owner.directory()/"study-v1.save",out.str())){notice="Not saved. Your session text is retained; check disk space and retry.";return false;}
 unsaved=false;notice="Saved on this Mac";return true;
}
bool StudyStore::saveSnapshot(const std::string& hash,const std::string& source){
 if(!writable() || source.size()>65536 || hash!=sourceHash(source))return false;
 auto folder=owner.directory()/"source-snapshots"/std::to_string(generation);std::error_code ec;
 std::filesystem::create_directories(folder,ec);if(ec)return false;
 auto path=folder/(hash+".cpp");int fd=open(path.c_str(),O_WRONLY|O_CREAT|O_EXCL|O_NOFOLLOW,0600);
 if(fd>=0){bool ok=writeBytes(fd,source);if(close(fd))ok=false;if(!ok)return false;return syncDirectory(folder)&&syncDirectory(folder.parent_path())&&syncDirectory(owner.directory());}
 if(errno!=EEXIST)return false;
 auto size=std::filesystem::file_size(path,ec);if(ec || size>65536)return false;
 std::ifstream in(path);std::string bytes((std::istreambuf_iterator<char>(in)),{});return sourceHash(bytes)==hash;
}
}
