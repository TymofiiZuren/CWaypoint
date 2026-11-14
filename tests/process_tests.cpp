#include "process.hpp"
#include <stdexcept>
#include <iostream>
#include <future>
#include <thread>
using namespace cw;
void check(bool b){if(!b)throw std::runtime_error("process contract failed");}
int main(){
 auto r=runProcess({"/bin/cat"},"input retained\n",2);check(r.exitCode==0 && r.output=="input retained\n");
 r=runProcess({"/usr/bin/yes"},"",2);check(r.outputLimited && r.output.size()<=65536);
 r=runProcess({"/bin/sleep","5"},"",.1);check(r.timedOut);
 std::atomic_bool cancel{true};r=runProcess({"/bin/sleep","5"},"",2,&cancel);check(r.cancelled);
 r=runProcess({"/nonexistent/cwaypoint-fixture"},"",2);check(r.exitCode==-1 && !r.error.empty());
 cancel.store(false);auto pending=std::async(std::launch::async,[&]{return runProcess({"/bin/sleep","5"},"",3,&cancel);});std::this_thread::sleep_for(std::chrono::milliseconds(50));cancel.store(true);check(pending.get().cancelled);
 std::cout<<"Process contracts passed\n";
}
