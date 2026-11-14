#import <Foundation/Foundation.h>
#include "process.hpp"
#include <atomic>
#include <csignal>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <sys/resource.h>
using namespace cw;
namespace {
std::atomic_bool stopped{false};
void stop(int){stopped.store(true);}
struct Request {std::string language,source;std::vector<std::string> inputs;};
bool readRequest(Request& r){
 std::string bytes;char c;while(std::cin.get(c)){if(bytes.size()>=270000)return false;bytes+=c;}std::istringstream inputStream(bytes);
 std::string magic;size_t count;
 if(!(inputStream>>magic>>r.language>>std::quoted(r.source)>>count))return false;
 if(magic!="CWLAB1" || r.source.empty() || r.source.size()>65536 || count==0 || count>16)return false;
 if(r.language!="c" && r.language!="cpp")return false;
 for(size_t i=0;i<count;++i){std::string input;if(!(inputStream>>std::quoted(input)) || input.size()>4096)return false;r.inputs.push_back(input);}
 return true;
}
void writeRun(const ProcessResult& r){
 std::cout<<r.exitCode<<' '<<r.timedOut<<' '<<r.cancelled<<' '<<r.outputLimited<<' '<<std::quoted(r.output)<<' '<<std::quoted(r.error)<<'\n';
}
std::filesystem::path workspace(){
 std::string base=[NSTemporaryDirectory() UTF8String];base+="cwaypoint-lab-XXXXXX";
 if(!mkdtemp(base.data()))return {};return base;
}
ProcessResult compile(const Request& r,const std::filesystem::path& tools,const std::filesystem::path& job){
 auto source=job/(r.language=="c" ? "main.c":"main.cpp");std::ofstream f(source);f<<r.source;f.close();
 auto compiler=tools/"wasi-sdk"/"bin"/(r.language=="c" ? "clang":"clang++");
 return runProcess({compiler.string(),r.language=="c" ? "-std=c17":"-std=c++17","-O0",source.string(),"-o",(job/"program.wasm").string()},"",15,&stopped,65536,false);
}
ProcessResult execute(const std::string& input,const std::filesystem::path& tools,const std::filesystem::path& job){
 return runProcess({(tools/"wasmtime").string(),"run","-Ccache=n","-Wfuel=10000000","-Wtimeout=2s","-Wmax-memory-size=67108864","-Sinherit-env=false","-Sinherit-network=false","-Sthreads=false",(job/"program.wasm").string()},input,3,&stopped,65536,false);
}
int work(const std::filesystem::path& tools){
 Request r;if(!readRequest(r)){std::cerr<<"Invalid bounded lab request.";return 2;}
 auto job=workspace();if(job.empty()){std::cerr<<"Cannot create isolated workspace.";return 3;}
 auto build=compile(r,tools,job);const bool ok=build.exitCode==0 && !build.timedOut && !stopped.load();
 std::vector<ProcessResult> results;
 if(ok)for(const auto& input:r.inputs){results.push_back(execute(input,tools,job));if(stopped.load())break;}
 auto message=build.error;
 if(!ok && message.empty())message="Compiler stopped with exit code "+std::to_string(build.exitCode)+".";
 if(build.timedOut)message="Compilation timed out.";
 std::cout<<"CWRESULT1 "<<ok<<' '<<std::quoted(message)<<' '<<results.size()<<'\n';
 for(const auto& result:results)writeRun(result);
 std::error_code error;std::filesystem::remove_all(job,error);return 0;
}
}
int main(int argc,char** argv){
 @autoreleasepool {
  if(argc!=2)return 2;
  std::signal(SIGTERM,stop);std::signal(SIGINT,stop);
  rlimit cpu{40,40};setrlimit(RLIMIT_CPU,&cpu);
  rlimit file{16*1024*1024,16*1024*1024};setrlimit(RLIMIT_FSIZE,&file);
  return work(argv[1]);
 }
}
