#include "study_store.hpp"
#include "lab.hpp"
#include <fstream>
#include <stdexcept>
#include <unistd.h>
using namespace cw;
void check(bool b){if(!b)throw std::runtime_error("study store contract");}
int main(){
 auto root=std::filesystem::path(".scratch")/("study-"+std::to_string(getpid()));Store owner(root);
 StudyStore s(owner);s.preferences.name="Tymofii";s.entries["c-variables"]={"A note\nwith two lines","int main(){}",true,true,3};check(s.save());
 StudyStore t(owner);check(t.preferences.name=="Tymofii" && t.entries.at("c-variables").note=="A note\nwith two lines");
 auto hash=sourceHash("int main(){}");check(t.saveSnapshot(hash,"int main(){}"));check(!t.saveSnapshot(hash,"different"));
 Profile p;check(owner.reset(p));t.syncGeneration();check(t.entries.empty() && t.preferences.name=="Tymofii");check(t.save());
 s.preferences.goal=99;check(!s.save());
 auto bad=root/"other";std::filesystem::create_directories(bad);{std::ofstream f(bad/"study-v1.save");f<<"CWSTUDY 99 1\n";}
 Store readonly(bad);StudyStore unsupported(readonly);check(!unsupported.writable() && !unsupported.save());
}
