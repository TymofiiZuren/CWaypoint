#include "curriculum.hpp"
#include "process.hpp"
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <unistd.h>
using namespace cw;
static void require(bool condition, const std::string& message) {
  if (!condition) throw std::runtime_error(message);
}
int main() {
  auto lessons = additionalLessons();
  require(validate(lessons).empty(), "New catalogue validation");
  const std::vector<std::string> outputs{"3\n", "5\n", "6\n", "3 6\n"};
  auto root = std::filesystem::temp_directory_path() /
      ("cwaypoint-content-" + std::to_string(getpid()));
  std::filesystem::create_directories(root);
  for (size_t i = 0; i < lessons.size(); ++i) {
    const auto& lesson = lessons[i];
    bool cpp = lesson.track == "C++";
    auto executable = root / lesson.id;
    std::string headers = cpp ? "#include <iostream>\n#include <vector>\n" : "#include <stdio.h>\n";
    std::string program = lesson.code;
    if (program.find("int main(") == std::string::npos)
      program = "int main(void) {\n" + program + "\nreturn 0;\n}";
    auto compile = runProcess({cpp ? "/usr/bin/clang++" : "/usr/bin/clang",
        cpp ? "-std=c++17" : "-std=c17", "-Wall", "-Werror", "-x", cpp ? "c++" : "c",
        "-", "-o", executable.string()}, headers + program, 15);
    require(compile.exitCode == 0, lesson.id + " example compilation: " + compile.error);
    auto run = runProcess({executable.string()}, "", 3);
    require(run.exitCode == 0 && run.output == outputs[i], lesson.id + " example output");
    auto lab = additionalLab(lesson.id);
    require(lab.language == (cpp ? "cpp" : "c"), "Lab language");
    compile = runProcess({cpp ? "/usr/bin/clang++" : "/usr/bin/clang",
        cpp ? "-std=c++17" : "-std=c17", "-Wall", "-Werror", "-x", cpp ? "c++" : "c",
        "-", "-o", executable.string()}, lab.solution, 15);
    require(compile.exitCode == 0, lesson.id + " lab compilation");
    for (const auto& test : lab.cases) {
      run = runProcess({executable.string()}, test.input, 3);
      require(run.exitCode == 0 && run.output == test.expected, lesson.id + " lab result");
    }
    const auto& fresh = lesson.questions[2];
    if (fresh.type == QuestionType::Output) {
      std::string freshProgram = fresh.code;
      if (freshProgram.find("int main(") == std::string::npos)
        freshProgram = "int main(void) {\n" + freshProgram + "\nreturn 0;\n}";
      compile = runProcess({cpp ? "/usr/bin/clang++" : "/usr/bin/clang",
          cpp ? "-std=c++17" : "-std=c17", "-x", cpp ? "c++" : "c",
          "-", "-o", executable.string()}, headers + freshProgram, 15);
      require(compile.exitCode == 0, lesson.id + " fresh compilation");
      run = runProcess({executable.string()}, "", 3);
      require(run.exitCode == 0 && evaluate(fresh, run.output).kind == Verdict::Correct,
              lesson.id + " fresh output agrees with answer");
    }
    for (const auto& question : lesson.questions) {
      require(evaluate(question, "").kind == Verdict::Invalid, "Empty input");
      require(evaluate(question, "definitely wrong").kind != Verdict::Correct, "Wrong input");
    }
  }
  std::filesystem::remove_all(root);
  std::cout << "Four authored examples and twelve lab cases verified by compiler\n";
}
