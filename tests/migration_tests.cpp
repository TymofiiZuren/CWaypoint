#include "core.hpp"
#include <fstream>
#include <stdexcept>
#include <unistd.h>
using namespace cw;
void check(bool v){if(!v)throw std::runtime_error("v3 to v4 migration contract");}
std::string bytes(const std::filesystem::path& p){std::ifstream f(p);return {(std::istreambuf_iterator<char>(f)),{}};}
int main(){
 auto root=std::filesystem::path(".scratch")/("migration-"+std::to_string(getpid()));
 auto lessons=sampleLessons();std::filesystem::path snapshot;std::string legacy;
 {Store store(root);Controller app(lessons,store);check(app.save());snapshot=root/("generation-"+std::to_string(store.activeGeneration())+".save");legacy=bytes(snapshot);legacy.replace(legacy.find("CWPROFILE 4"),11,"CWPROFILE 3");legacy.erase(legacy.find("LAB "));legacy+="END\n";std::ofstream f(snapshot);f<<legacy;}
 {Store store(root);Controller app(lessons,store);check(app.enableLabMode());check(bytes(snapshot.string()+".pre-v4")==legacy);check(bytes(snapshot).find("CWPROFILE 4")==0);check(app.save());check(bytes(snapshot.string()+".pre-v4")==legacy);}
}
