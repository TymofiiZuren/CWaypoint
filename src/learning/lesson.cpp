#include "lesson.hpp"
#include <algorithm>
#include <cctype>
namespace cwaypoint {
static std::string trim(std::string s) {
    const auto first=s.find_first_not_of(" \t\r\n");
    if(first==std::string::npos)return {};
    return s.substr(first,s.find_last_not_of(" \t\r\n")-first+1);
}
static std::string output(std::string s) {
    for(std::size_t p=0;(p=s.find("\r\n",p))!=std::string::npos;)s.replace(p,2,"\n");
    if(!s.empty() && s.back()=='\n')s.pop_back();
    return s;
}
Result evaluate(const Exercise& e,const std::string& answer) {
    if(e.id.empty() || e.accepted.empty() || trim(answer).empty())return Result::Invalid;
    switch(e.type) {
    case ExerciseType::SingleChoice:
        break;
    case ExerciseType::FillToken:
        for(const auto& expected:e.accepted)if(trim(answer)==expected)return Result::Correct;
        return Result::Incorrect;
    case ExerciseType::PredictOutput:
        for(const auto& expected:e.accepted)if(output(answer)==output(expected))return Result::Correct;
        return Result::Incorrect;
    default:return Result::Invalid;
    }
    return std::find(e.accepted.begin(),e.accepted.end(),answer)!=e.accepted.end()?Result::Correct:Result::Incorrect;
}
Lesson variablesLesson() {
    return {"c.variables","Variables and assignment",
        "An int stores a whole number. Assignment replaces its value. In score = score + 2, the right side uses the old value before the new value is stored.",
        "int score = 3;\nscore = score + 2;\nprintf(\"%d\\n\", score);",1,{
        {"c.variables.output","What does the example print?",ExerciseType::PredictOutput,{"5\n"},
         "score starts at 3; adding 2 stores 5. printf prints 5 followed by a newline.",
         {"Trace the statements in order.","The right side adds 2 to the current score of 3.","3 + 2 is 5, so the output is 5."},false},
        {"c.variables.token","Fill the type: ___ score = 3;",ExerciseType::FillToken,{"int"},
         "int declares an integer variable.",{"We need a whole-number type.","Use the basic signed integer type.","The declaration is int score = 3;."},false},
        {"c.variables.fresh","int a = 4; int b = a; a = 9; What does printf(\"%d\\n\", b) print?",ExerciseType::PredictOutput,{"4\n"},
         "b receives a copy of 4. Changing a later does not change b.",
         {"Are a and b separate variables?","b copies the value at its declaration.","b remains 4 even after a becomes 9."},true}
    }};
}
}
