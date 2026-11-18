#include "curriculum.hpp"
#include "lab.hpp"
namespace cw {
static Lesson lesson0() {
  return {"c-loops", "C", "Loops and repetition", "Trace a for loop and its stopping condition.",
    "A for loop has an initial value, a condition and an update. The condition is checked before each iteration. With i < 3, the body runs for 0, 1 and 2. It stops when i becomes 3. A loop may run zero times.", "int total = 0;\nfor (int i = 0; i < 3; ++i) {\n    total += i;\n}\nprintf(\"%d\\n\", total);", {
    {"c-loops-p1", "What does the example print?", "3", {}, {"List the values of i.", "Add 0, then 1, then 2 to total.", "0 + 1 + 2 equals 3."}, QuestionType::Output, "The loop excludes 3, so total is 3.", ""},
    {"c-loops-p2", "Which condition visits i = 0, 1, 2 and 3?", "i <= 3", {"i < 3", "i <= 3", "i > 3"}, {"Check whether equality is included.", "The last allowed value must pass the test.", "i <= 3 includes i equal to 3."}, QuestionType::Choice, "The <= operator includes the upper boundary.", ""},
    {"c-loops-fresh", "What does this loop print after it finishes?", "0", {}, {"Check the condition before running the body.", "i starts at 5, and 5 < 5 is false.", "The loop executes zero times, so count stays 0."}, QuestionType::Output, "The first condition fails, so the body is skipped.", "int count = 0;\nfor (int i = 5; i < 5; ++i) { ++count; }\nprintf(\"%d\\n\", count);"}
    }, {
      {"int total = 0;", "total = 0", "Initialise the accumulator."},
      {"i = 0; total += i;", "total = 0", "The first iteration adds zero."},
      {"i = 1; total += i;", "total = 1", "The second iteration adds one."},
      {"i = 2; total += i;", "total = 3", "The third iteration adds two. i becomes 3 and the condition fails."}
    }};
}
static LabExercise lab0() {
  return {"c-loops", "c", "#include <stdio.h>\nint main(void) {\n    int n = 0, sum = 0;\n    if (scanf(\"%d\", &n) != 1 || n < 0 || n > 100) return 1;\n    for (int i = 1; i <= n; ++i) { sum += 0; }\n    printf(\"%d\\n\", sum);\n    return 0;\n}\n", "#include <stdio.h>\nint main(void) {\n    int n = 0, sum = 0;\n    if (scanf(\"%d\", &n) != 1 || n < 0 || n > 100) return 1;\n    for (int i = 1; i <= n; ++i) { sum += i; }\n    printf(\"%d\\n\", sum);\n    return 0;\n}\n", "Read n from 0 to 100. Print the sum of the integers from 1 through n. Include n in the sum.", 1, {{"0\n", "0\n"}, {"1\n", "1\n"}, {"5\n", "15\n"}}};
}
static Lesson lesson1() {
  return {"c-functions", "C", "Functions and return values", "Pass values to a function and use its result.",
    "A function packages a calculation. Parameters receive copies of argument values in C. return sends a result to the caller. Assign that result if you want to update a caller variable. Changing a plain int parameter does not change the original int.", "int addTwo(int value) {\n    return value + 2;\n}\nint main(void) {\n    int score = 3;\n    score = addTwo(score);\n    printf(\"%d\\n\", score);\n}", {
    {"c-functions-p1", "What does the example print?", "5", {}, {"Find the argument passed to addTwo.", "The parameter receives 3 and the function returns 3 + 2.", "The returned 5 is assigned to score."}, QuestionType::Output, "The caller stores the returned result 5.", ""},
    {"c-functions-p2", "Which keyword sends a value back to the caller?", "return", {}, {"Look at the function body.", "The keyword precedes value + 2.", "return sends the result back."}, QuestionType::Token, "return supplies the function result.", ""},
    {"c-functions-fresh", "What is printed after change(n)?", "4", {}, {"Consider whether the parameter is a copy.", "Assigning to value changes only the local parameter.", "n remains 4 because no result is assigned to it."}, QuestionType::Output, "The function changes its copy, so n remains 4.", "void change(int value) { value = 9; }\nint main(void) { int n = 4; change(n); printf(\"%d\\n\", n); }"}
    }, {
      {"score = 3;", "score = 3", "The caller owns score."},
      {"addTwo(score)", "value = 3", "The function receives a copy."},
      {"return value + 2;", "result = 5", "Compute the returned value."},
      {"score = addTwo(score);", "score = 5", "The caller stores the result."}
    }};
}
static LabExercise lab1() {
  return {"c-functions", "c", "#include <stdio.h>\nint square(int value) { return value; }\nint main(void) {\n    int n = 0;\n    if (scanf(\"%d\", &n) != 1 || n < -100 || n > 100) return 1;\n    printf(\"%d\\n\", square(n));\n    return 0;\n}\n", "#include <stdio.h>\nint square(int value) { return value * value; }\nint main(void) {\n    int n = 0;\n    if (scanf(\"%d\", &n) != 1 || n < -100 || n > 100) return 1;\n    printf(\"%d\\n\", square(n));\n    return 0;\n}\n", "Read an integer from -100 to 100. Implement square(value) and print its returned square.", 1, {{"0\n", "0\n"}, {"-3\n", "9\n"}, {"4\n", "16\n"}}};
}
static Lesson lesson2() {
  return {"cpp-references", "C++", "References and const", "Distinguish a reference from a copied value.",
    "A reference is another name for an existing object. In int& alias = value, assignment through alias changes value. A const int& permits reading through the reference but prevents assignment through it. A reference must be initialised and its target must remain alive.", "int value = 4;\nint& alias = value;\nalias += 2;\nstd::cout << value << \"\\n\";", {
    {"cpp-references-p1", "What does the example print?", "6", {}, {"Determine which object alias names.", "alias and value name the same integer.", "Adding 2 through alias changes value from 4 to 6."}, QuestionType::Output, "The reference updates the original integer.", ""},
    {"cpp-references-p2", "Fill the reference declaration: int___ alias = value;", "&", {}, {"A reference declaration uses a symbol after the type.", "This symbol also appears in address expressions, but this is a declaration.", "int& alias = value declares a reference."}, QuestionType::Token, "The & after the type declares a reference.", ""},
    {"cpp-references-fresh", "Which statement is valid with const int& view = value?", "std::cout << view;", {"view = 8;", "++view;", "std::cout << view;"}, {"const limits modification through view.", "Reading through view is allowed.", "Printing view reads its value without modifying it."}, QuestionType::Choice, "A const reference permits reading but blocks assignment through that reference.", "int value = 4;\nconst int& view = value;"}
    }, {
      {"int value = 4;", "value = 4", "Create the integer."},
      {"int& alias = value;", "alias -> value = 4", "Bind an additional name to value."},
      {"alias += 2;", "alias -> value = 6", "Both names refer to the updated integer."}
    }};
}
static LabExercise lab2() {
  return {"cpp-references", "cpp", "#include <iostream>\nvoid increment(int& value) { value += 0; }\nint main() {\n    int n = 0;\n    if (!(std::cin >> n) || n < -100 || n > 100) return 1;\n    increment(n);\n    std::cout << n << \"\\n\";\n    return 0;\n}\n", "#include <iostream>\nvoid increment(int& value) { value += 1; }\nint main() {\n    int n = 0;\n    if (!(std::cin >> n) || n < -100 || n > 100) return 1;\n    increment(n);\n    std::cout << n << \"\\n\";\n    return 0;\n}\n", "Read a value from -100 to 100. Implement increment(int& value) to increase the original value by one, then print it.", 1, {{"-1\n", "0\n"}, {"0\n", "1\n"}, {"8\n", "9\n"}}};
}
static Lesson lesson3() {
  return {"cpp-vectors", "C++", "Vectors and indexing", "Grow a vector and access valid element positions.",
    "std::vector stores a sequence of elements and manages its storage. Include <vector>. push_back appends an element and size() reports the count. Positions start at zero. at() checks the position and throws for an invalid index. Keep every index below size().", "std::vector<int> values{2, 4};\nvalues.push_back(6);\nstd::cout << values.size() << \" \" << values.at(2) << \"\\n\";", {
    {"cpp-vectors-p1", "What does the example print?", "3 6", {}, {"Count the elements after push_back.", "There are three elements, and index 2 is the third.", "size() is 3 and values.at(2) is 6."}, QuestionType::Output, "Appending 6 gives {2, 4, 6}, so the output is 3 6.", ""},
    {"cpp-vectors-p2", "Which index refers to the first element?", "0", {"0", "1", "values.size()"}, {"C++ uses zero-based indexing.", "The first position is one less than 1.", "Index 0 selects the first element."}, QuestionType::Choice, "The first vector element has index 0.", ""},
    {"cpp-vectors-fresh", "What does this different vector print?", "9", {}, {"Apply the assignment to the indexed element.", "Index 1 selects 7, then replaces it with 9.", "at(1) now reads 9."}, QuestionType::Output, "Changing an existing element keeps the vector size and updates that position.", "std::vector<int> v{5, 7};\nv.at(1) = 9;\nstd::cout << v.at(1) << \"\\n\";"}
    }, {
      {"std::vector<int> values{2, 4};", "values = [2, 4] | size = 2", "Construct two elements."},
      {"values.push_back(6);", "values = [2, 4, 6] | size = 3", "Append one element."},
      {"values.at(2)", "value = 6", "Read the third element with index 2."}
    }};
}
static LabExercise lab3() {
  return {"cpp-vectors", "cpp", "#include <iostream>\n#include <vector>\nint main() {\n    std::vector<int> values;\n    for (int i = 0; i < 3; ++i) {\n        int value = 0;\n        if (!(std::cin >> value) || value < -100 || value > 100) return 1;\n        values.push_back(value);\n    }\n    int total = 0;\n    for (int value : values) { total += 1; }\n    std::cout << total << \"\\n\";\n    return 0;\n}\n", "#include <iostream>\n#include <vector>\nint main() {\n    std::vector<int> values;\n    for (int i = 0; i < 3; ++i) {\n        int value = 0;\n        if (!(std::cin >> value) || value < -100 || value > 100) return 1;\n        values.push_back(value);\n    }\n    int total = 0;\n    for (int value : values) { total += value; }\n    std::cout << total << \"\\n\";\n    return 0;\n}\n", "Read three integers, each between -100 and 100. Store them in a vector and print their sum.", 1, {{"1 2 3\n", "6\n"}, {"-4 0 4\n", "0\n"}, {"0 0 0\n", "0\n"}}};
}
std::vector<Lesson> additionalLessons() { return {lesson0(), lesson1(), lesson2(), lesson3()}; }
LabExercise additionalLab(const std::string& id) {
  if (id == "c-loops") return lab0();
  if (id == "c-functions") return lab1();
  if (id == "cpp-references") return lab2();
  if (id == "cpp-vectors") return lab3();
  return {};
}
} // namespace cw
