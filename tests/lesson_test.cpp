#include "lesson.hpp"
#include <cassert>
#include <iostream>
using namespace cwaypoint;
int main() {
    auto l=variablesLesson();
    assert(l.exercises.size()==3);
    auto e=l.exercises[0];
    assert(evaluate(e,"5")==Result::Correct);
    assert(evaluate(e,"5\r\n")==Result::Correct);
    assert(evaluate(e,"5\n\n")==Result::Incorrect);
    assert(evaluate(e," 5")==Result::Incorrect);
    assert(evaluate(e,"")==Result::Invalid);
    assert(evaluate(e,"3")==Result::Incorrect);
    assert(evaluate(l.exercises[1]," int \t")==Result::Correct);
    assert(evaluate(l.exercises[1],"INT")==Result::Incorrect);
    assert(evaluate(l.exercises[2],"4")==Result::Correct);
    assert(evaluate(l.exercises[2],"9")==Result::Incorrect);
    e.accepted.clear();assert(evaluate(e,"5")==Result::Invalid);
    for(const auto& q:l.exercises)assert(q.hints.size()==3);
    std::cout << "Lesson evaluator checks passed.\n";
}
