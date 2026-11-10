#pragma once
#include "core.hpp"
#include "learning/lesson.hpp"
namespace cw {
// Deliberately bounded first-handoff adapter. Throws on unsupported records.
Lesson adaptVariablesLesson(const cwaypoint::Lesson &);
}
