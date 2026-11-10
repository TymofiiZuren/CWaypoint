#pragma once
#include <array>
#include <filesystem>
#include <map>
#include <set>
#include <string>
#include <vector>
namespace cw {
enum class QuestionType { Choice, Output, Token };
struct Question {
  std::string id, prompt, answer;
  std::vector<std::string> choices;
  std::array<std::string, 3> hints;
  QuestionType type = QuestionType::Token;
  std::string explanation;
  std::string code = {}; // Optional question-specific context; no learner execution.
};
struct Step {
  std::string code, state, explanation;
};
struct Lesson {
  std::string id, track, title, objective, body, code;
  std::vector<Question> questions;
  std::vector<Step> steps;
  int revision = 1;
};
struct Comparison {
  std::string title, c, cpp, explanation;
};
std::vector<Lesson> sampleLessons();
std::vector<Comparison> comparisons();
std::string validate(const std::vector<Lesson> &);
enum class Verdict { Invalid, Incorrect, Correct };
struct Result {
  Verdict kind;
  std::string message;
};
std::string validateWalkthrough(const std::vector<Step> &);
std::string validateComparison(const Comparison &);
Result evaluate(const Question &, const std::string &);
struct Evidence {
  int attempts = 0;
  bool issued = false, helped = false, solved = false, independent = false;
  int revision = 1;
};
int localStudyDay();
int studyStreak(const std::set<int>& days, int today);
struct Profile {
  std::set<int> studyDays;
  std::map<std::string, Evidence> evidence;
  std::set<std::string> review;
  std::string lesson;
  int step = 0, textSize = 18;
  int question = 0, walkthrough = 0, resumeRevision = 0;
  bool recovered = false;
};
class Store {
  std::filesystem::path root;
  int lockFd = -1;
  unsigned long generation = 1;
  bool canWrite = false;
  std::string message;
  bool writeSnapshot(const Profile &);
  bool readSnapshot(const std::filesystem::path &, Profile &);

public:
  explicit Store(std::filesystem::path);
  ~Store();
  Store(const Store &) = delete;
  Store &operator=(const Store &) = delete;
  Profile load();
  bool save(const Profile &);
  bool reset(Profile &);
  bool writable() const { return canWrite; }
  const std::string &status() const { return message; }
  const std::filesystem::path &directory() const { return root; }
};
struct ReviewItem {
  int lesson, question;
};
class Controller {
  const std::vector<Lesson> &lessons;
  Store &store;
  std::string submitted;
  bool pendingSubmission = false;
  bool persist();

public:
  Profile profile;
  std::string notice;
  bool unsavedChanges = false;
  Controller(const std::vector<Lesson> &, Store &);
  bool issue(int lesson, int question);
  bool help(int lesson, int question);
  Result submit(int lesson, int question, const std::string &);
  void retry();
  std::string status(int lesson, int question) const;
  std::vector<ReviewItem> reviewItems(const std::string &track = "") const;
  bool solved(int lesson, int question) const;
  int completed(int lesson) const;
  int nextLesson(const std::string &track, int after = -1) const;
  bool resume(int lesson, int step, int question = 0, int walkthrough = 0);
  bool setTextSize(int size);
  bool reset();
  bool save() { return persist(); }
};
} // namespace cw
