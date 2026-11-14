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
 auto loaded=store.load();check(loaded.achievements.size()==1 && loaded.completionDays.size()==1);
 r.sourceHash="bad";check(!app.completeLesson(0,r,store.activeGeneration(),101));
 auto gen=store.activeGeneration();check(app.reset());check(!app.completeLesson(0,r,gen,101));
 check(app.profile.achievements.empty());std::cout<<"Durable completion contracts passed\n";
}
