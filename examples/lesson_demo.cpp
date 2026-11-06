#include "lesson.hpp"
#include <iostream>
int main() {
    auto lesson=cwaypoint::variablesLesson();
    std::cout<<lesson.title<<"\n\n"<<lesson.explanation<<"\n\n"<<lesson.code<<"\n";
    for(const auto& e:lesson.exercises) {
        std::cout<<"\n"<<(e.fresh_check?"Fresh check: ":"Practice: ")<<e.prompt<<"\nAnswer: ";
        std::string answer;if(!std::getline(std::cin,answer))break;
        auto result=cwaypoint::evaluate(e,answer);
        std::cout<<(result==cwaypoint::Result::Correct?"Correct":result==cwaypoint::Result::Invalid?"Enter a nonempty answer":"Incorrect")<<"\n"<<e.explanation<<"\n";
    }
}
