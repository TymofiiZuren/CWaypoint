#include "core.hpp"
#include "lesson_adapter.hpp"
#include <iostream>
#include <stdexcept>
#include <unistd.h>
using namespace cw;
void require(bool ok, const char* message) { if (!ok) throw std::runtime_error(message); }
int main() {
 try {
  auto source = cwaypoint::variablesLesson();
  auto lesson = adaptVariablesLesson(source);
  require(lesson.id == "c-variables" && lesson.revision == 2, "stable application identity and new revision");
  require(lesson.title == source.title && lesson.body == source.explanation && lesson.code == source.code, "authored teaching text retained");
  require(validate({lesson}).empty(), "adapted lesson contract");
  require(lesson.questions[2].code.find("int b = a;") != std::string::npos, "fresh check has matching code context");
  require(validateWalkthrough(lesson.steps).empty(), "existing walkthrough retained");
  for (size_t i=0;i<3;++i) {
   const auto &q=lesson.questions[i]; const auto &e=source.exercises[i];
   require(q.prompt==e.prompt && q.answer==e.accepted[0] && q.explanation==e.explanation, "question content retained");
   for(size_t h=0;h<3;++h) require(q.hints[h]==e.hints[h], "all hints retained");
   for(const auto &answer: std::vector<std::string>{"", " ", "5", "5\n", "5\r\n", "5\n\n", " 5", "int", " int \t", "INT", "4", "4\r\n", "wrong"}) {
    auto actual=evaluate(q,answer).kind;
    auto expected=cwaypoint::evaluate(e,answer);
    require((actual==Verdict::Correct)==(expected==cwaypoint::Result::Correct) && (actual==Verdict::Invalid)==(expected==cwaypoint::Result::Invalid), "evaluator parity");
   }
  }
  auto rejects=[&](auto mutate) { auto bad=source; mutate(bad); bool rejected=false; try { adaptVariablesLesson(bad); } catch(const std::invalid_argument&) { rejected=true; } require(rejected,"unsupported authored record rejected"); };
  rejects([](auto &l){l.id="other";});
  rejects([](auto &l){l.title.clear();});
  rejects([](auto &l){l.explanation=" ";});
  rejects([](auto &l){l.code.clear();});
  rejects([](auto &l){l.code += "\nscore = 10;";});
  rejects([](auto &l){l.assessment_revision=0;});
  rejects([](auto &l){l.assessment_revision=2;});
  rejects([](auto &l){l.exercises.pop_back();});
  rejects([](auto &l){l.exercises[1].id=l.exercises[0].id;});
  rejects([](auto &l){l.exercises[0].prompt.clear();});
  rejects([](auto &l){l.exercises[0].explanation.clear();});
  rejects([](auto &l){l.exercises[0].accepted.push_back("05");});
  rejects([](auto &l){l.exercises[0].accepted[0]=" ";});
  rejects([](auto &l){l.exercises[0].accepted[0]=std::string(1025, '5');});
  rejects([](auto &l){l.exercises[0].hints.pop_back();});
  rejects([](auto &l){l.exercises[0].hints[0].clear();});
  rejects([](auto &l){l.exercises[0].fresh_check=true;});
  rejects([](auto &l){l.exercises[2].fresh_check=false;});
  rejects([](auto &l){l.exercises[0].type=cwaypoint::ExerciseType::SingleChoice;});
  rejects([](auto &l){l.exercises[0].type=cwaypoint::ExerciseType::FillToken;});
  auto catalogue=sampleLessons();
  require(catalogue[0].body==source.explanation && catalogue[0].revision==2 && catalogue.size()==3, "catalogue uses authored source");
  auto path=std::filesystem::temp_directory_path()/("cw-adapter-"+std::to_string(getpid()));
  auto old=catalogue; old[0].revision=1;
  { Store store(path); Controller c(old,store);
   c.submit(0,0,"wrong"); c.retry(); c.submit(0,1,"int"); c.resume(0,1,1,2);
  }
  { Store store(path); Controller c(catalogue,store);
   require(c.completed(0)==0 && c.reviewItems().empty() && c.profile.step==0 && c.profile.question==0 && c.profile.walkthrough==0, "old revision cannot transfer progress or resume");
   require(c.issue(0,0) && c.help(0,0),"persist issuance and help");
   require(c.submit(0,0,"wrong").kind==Verdict::Incorrect,"wrong authored answer"); c.retry();
   require(c.submit(0,0,"5").kind==Verdict::Correct && c.status(0,0)=="Solved with practice" && c.reviewItems().empty(),"assisted retry clears review");
   c.submit(0,1,"int"); c.issue(0,2); c.submit(0,2,"4"); c.resume(0,1,1);
  }
  { Store store(path); Controller c(catalogue,store); require(c.completed(0)==2 && c.solved(0,2) && c.profile.question==1,"integrated restart"); }
  std::cout<<"Authored lesson integration passed\n";
 } catch(const std::exception &e) { std::cerr<<e.what()<<'\n'; return 1; }
}
