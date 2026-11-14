#include "lab.hpp"
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <unistd.h>
using namespace cw;
void check(bool b,const char* why){if(!b)throw std::runtime_error(why);}
int main(int argc,char**argv){
 check(argc==3,"helper/tools arguments");
 for(const auto& id:{"c-variables","cpp-strings","c-pointers","c-conditions"}){
  auto e=labFor(id);auto r=runLab(e,e.solution,argv[1],argv[2]);
  if(!r.allPassed()){std::cerr<<id<<": compiled="<<r.compiled<<" tests="<<r.runs.size()<<" "<<r.diagnostics<<"\n";for(auto& run:r.runs)std::cerr<<run.exitCode<<" "<<run.error<<" "<<run.output<<"\n";}
  check(r.allPassed(),"real isolated language fixture failed");
  auto initial=runLab(e,e.starter,argv[1],argv[2]);check(initial.compiled && !initial.allPassed(),"starter must need an edit");
 }
 auto e=labFor("c-variables");auto r=runLab(e,"invalid source",argv[1],argv[2]);check(!r.compiled,"invalid compile");
 r=runLab(e,"int main(){while(1){}}",argv[1],argv[2]);check(r.compiled && !r.allPassed(),"loop bounded");
 e.cases={{"",""}};
 std::string denied="#include <stdio.h>\n#include <stdlib.h>\nint main(){if(fopen(\"/etc/passwd\",\"r\"))return 9;if(getenv(\"HOME\"))return 8;return 0;}";
 r=runLab(e,denied,argv[1],argv[2]);check(r.allPassed(),"guest file/environment access denied");
 auto fixture=std::filesystem::absolute(std::filesystem::path(".scratch")/("denied source "+std::to_string(getpid())+".h"));std::filesystem::create_directories(fixture.parent_path());{std::ofstream f(fixture);f<<"#define REAL_FIXTURE 1\n";}
 std::string compileRead="#include \""+fixture.string()+"\"\nint main(){return 0;}";
 r=runLab(e,compileRead,argv[1],argv[2]);check(!r.compiled,"compiler must not read unrelated workspace");
 r=runLab(e,std::string(65537,'x'),argv[1],argv[2]);check(!r.compiled,"oversized source rejected");
 r=runLab(e,"#include <stdio.h>\nint main(){for(;;)puts(\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\");}",argv[1],argv[2]);check(r.compiled&&!r.allPassed(),"guest excessive output bounded");
 r=runLab(e,"#include <stdlib.h>\nint main(){return malloc(100*1024*1024)?9:0;}",argv[1],argv[2]);check(r.allPassed(),"guest memory allocation capped");
 r=runLab(e,"#include <stdio.h>\nint main(){return fopen(\"/tmp/cwaypoint-guest-write\",\"w\")?9:0;}",argv[1],argv[2]);check(r.allPassed(),"guest host writes denied");
 r=runLab(e,"// ; $(echo not-a-shell) & | >\nint main(){return 0;}",argv[1],argv[2]);check(r.allPassed(),"source is not shell interpreted");
 std::atomic_bool cancel{true};r=runLab(e,e.solution,argv[1],argv[2],&cancel);check(r.cancelled,"cancelled request");
 std::cout<<"Real C/C++ WASI fixtures and isolation checks passed\n";
}
