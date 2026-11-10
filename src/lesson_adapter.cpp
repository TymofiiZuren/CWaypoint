#include "lesson_adapter.hpp"
#include <stdexcept>
namespace cw {
namespace {
bool blank(const std::string &s) {
  return s.find_first_not_of(" \t\r\n") == std::string::npos;
}
void require(bool ok) {
  if (!ok)
    throw std::invalid_argument("Unsupported authored variables lesson contract");
}
} // namespace
Lesson adaptVariablesLesson(const cwaypoint::Lesson &source) {
  // Author revision 1 maps intentionally to application assessment revision 2.
  // Review this mapping when the author changes assessment content.
  require(source.id == "c.variables" && source.assessment_revision == 1 &&
          !blank(source.title) && !blank(source.explanation) &&
          !blank(source.code) && source.exercises.size() == 3);
  Lesson lesson;
  lesson.id = "c-variables";
  lesson.track = "C";
  lesson.title = source.title;
  lesson.objective = "Follow how assignment changes a stored value.";
  lesson.body = source.explanation;
  lesson.code = source.code;
  lesson.revision = 2;
  const std::array<std::string, 3> authoredIds{
      "c.variables.output", "c.variables.token", "c.variables.fresh"};
  const std::array<std::string, 3> applicationIds{
      "c-var-p1", "c-var-p2", "c-var-fresh"};
  for (size_t i = 0; i < 3; ++i) {
    const auto &exercise = source.exercises[i];
    auto expectedType = i == 1 ? cwaypoint::ExerciseType::FillToken
                              : cwaypoint::ExerciseType::PredictOutput;
    require(exercise.id == authoredIds[i] && exercise.type == expectedType &&
            exercise.fresh_check == (i == 2) &&
            exercise.accepted.size() == 1 &&
            !blank(exercise.accepted[0]) && exercise.accepted[0].size() <= 1024 &&
            !blank(exercise.prompt) &&
            !blank(exercise.explanation) && exercise.hints.size() == 3);
    Question question;
    question.id = applicationIds[i];
    question.prompt = exercise.prompt;
    question.answer = exercise.accepted[0];
    question.explanation = exercise.explanation;
    question.type = i == 1 ? QuestionType::Token : QuestionType::Output;
    for (size_t h = 0; h < 3; ++h) {
      require(!blank(exercise.hints[h]));
      question.hints[h] = exercise.hints[h];
    }
    lesson.questions.push_back(question);
  }
  lesson.questions[2].code = "int a = 4;\nint b = a;\na = 9;\nprintf(\"%d\\n\", b);";
  // This walkthrough describes this exact example; reject drift explicitly.
  require(source.code == "int score = 3;\nscore = score + 2;\nprintf(\"%d\\n\", score);");
  lesson.steps = {
      {"int score = 3;", "score = 3",
       "Declaration creates an integer and gives it the value 3."},
      {"score = score + 2;", "score = 5",
       "Read score on the right, add 2, then replace its stored value."},
      {"printf(\"%d\\n\", score);", "score = 5 | output: 5",
       "Output reads the current value without changing it."}};
  require(validate({lesson}).empty());
  return lesson;
}
} // namespace cw
