#pragma once
#include "core.hpp"
#include "lab.hpp"
namespace cw {
std::vector<Lesson> additionalLessons();
LabExercise additionalLab(const std::string& id);
}
