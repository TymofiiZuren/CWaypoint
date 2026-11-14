#include "core.hpp"
#include "lab.hpp"
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <unistd.h>
using namespace cw;
void check(bool b){if(!b)throw std::runtime_error("completion contract");}
int main(){
 auto path=std::filesystem::path(".scratch")/("completion-"+std::to_string(getpid()));
 auto lessons=sampleLessons();Store store(path);Controller app(lessons,store);app.enableLabMode();
 auto exercise=labFor(lessons[0].id);LabResult r;r.compiled=true;r.passed=2;r.runs.resize(2);r.sourceHash=sourceHash(exercise.solution);
 check(!app.completeLesson(0,r,store.activeGeneration(),100));
 for(int q=0;q<2;++q){app.retry();app.submit(0,q,lessons[0].questions[q].answer);}
 check(app.profile.studyDays.empty());
 r.testRevision=99;check(!app.completeLesson(0,r,store.activeGeneration(),100));r.testRevision=exercise.revision;
 check(app.completeLesson(0,r,store.activeGeneration(),100));check(app.lessonCompleted(0));
 check(app.completeLesson(0,r,store.activeGeneration(),100));check(app.completedToday(100)==1);
 check(app.submitEdited(0,2,lessons[0].questions[2].answer).kind==Verdict::Correct);
 check(app.completedToday()==0);
 auto loaded=store.load();check(loaded.achievements.size()==1 && loaded.completionDays.size()==1);
 r.sourceHash="bad";check(!app.completeLesson(0,r,store.activeGeneration(),101));
 auto gen=store.activeGeneration();check(app.reset());check(!app.completeLesson(0,r,gen,101));
 check(app.profile.achievements.empty());
 check(app.submitEdited(0,0,"wrong").kind==Verdict::Incorrect);
 check(app.submitEdited(0,0,lessons[0].questions[0].answer).kind==Verdict::Correct);
 auto attempts=app.profile.evidence.at(lessons[0].questions[0].id).attempts;
 check(app.submitEdited(0,0,lessons[0].questions[0].answer).kind==Verdict::Correct);
 check(app.submitEdited(0,0,"").kind==Verdict::Invalid);
 check(app.submitEdited(0,0,lessons[0].questions[0].answer).kind==Verdict::Correct);
 check(app.profile.evidence.at(lessons[0].questions[0].id).attempts==attempts);
 check(app.learningUnits(0)==1&&app.courseProgress("C")==11);check(app.reset());
 r.sourceHash=sourceHash(exercise.solution);
 check(app.recordLabResult(0,r,store.activeGeneration()));
 check(app.labPassed(0)&&app.learningUnits(0)==1&&!app.lessonCompleted(0));
 check(app.completedToday()==0);
 {Controller partialRestart(lessons,store);check(partialRestart.labPassed(0)&&partialRestart.learningUnits(0)==1&&!partialRestart.lessonCompleted(0));}
 for(int q=0;q<2;++q)check(app.submitEdited(0,q,lessons[0].questions[q].answer).kind==Verdict::Correct);
 check(app.learningUnits(0)==3&&app.lessonCompleted(0)&&app.completedToday()==1);
 auto persisted=store.load();check(persisted.achievements.size()==1&&persisted.completionDays.size()==1);
 Controller restarted(lessons,store);check(restarted.learningUnits(0)==3&&restarted.courseProgress("C")==33);
auto failing=path/"save-failure";Store failedStore(failing);Controller failed(lessons,failedStore);
 check(failed.issue(0,0));std::filesystem::create_directory(failing/"generation-1.save.tmp");
 check(failed.submitEdited(0,0,lessons[0].questions[0].answer).kind==Verdict::Invalid);
 check(failed.submitEdited(0,0,"different").kind==Verdict::Invalid);
 std::filesystem::rename(failing/"generation-1.save.tmp",failing/"retained-obstruction");check(failed.save());
 check(failed.submitEdited(0,0,lessons[0].questions[0].answer).kind==Verdict::Correct);
 check(failed.profile.evidence.at(lessons[0].questions[0].id).attempts==1);
std::cout<<"Durable completion contracts passed\n";
}
