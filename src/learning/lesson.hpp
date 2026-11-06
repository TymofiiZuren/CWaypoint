#pragma once
#include <string>
#include <vector>
namespace cwaypoint {
enum class ExerciseType { SingleChoice, PredictOutput, FillToken };
enum class Result { Correct, Incorrect, Invalid };
struct Exercise {
    std::string id, prompt;
    ExerciseType type;
    std::vector<std::string> accepted;
    std::string explanation;
    std::vector<std::string> hints;
    bool fresh_check;
};
struct Lesson {
    std::string id, title, explanation, code;
    unsigned assessment_revision;
    std::vector<Exercise> exercises;
};
Lesson variablesLesson();
Result evaluate(const Exercise& exercise, const std::string& answer);
}
