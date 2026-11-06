#include "core.hpp"
namespace cw {
std::vector<Lesson> sampleLessons() {
  return {
      {"c-variables",
       "C",
       "Variables & values",
       "Follow how assignment changes a stored value.",
       "A variable names a value in memory. In C, declare its type before "
       "using it. An assignment replaces the previous value: the expression on "
       "the right is evaluated first, then stored on the left.\n\nRead the "
       "example, predict its output, then inspect each step in the "
       "walkthrough. These exercises check a specific answer; the app does not "
       "compile your input.",
       "int score = 3;\nscore = score + 2;\nprintf(\"%d\\n\", score);",
       {{"c-var-p1",
         "What is score after both assignments?",
         "5",
         {"3", "5", "2"},
         {"Follow the statements in order.",
          "The second line adds 2 to the existing score.",
          "score begins at 3. Assignment stores 3 + 2, so score is 5."},
         QuestionType::Choice,
         "The second assignment stores 3 + 2, which is 5."},
        {"c-var-p2",
         "Fill the missing type: ___ score = 3;",
         "int",
         {},
         {"This variable stores a whole number.",
          "C's basic signed integer type has three letters.",
          "int score = 3; declares an integer named score."},
         QuestionType::Token,
         "int declares a whole-number variable."},
        {"c-var-fresh",
         "A new example: int n = 7; n = n - 3; What is n?",
         "4",
         {},
         {"Apply the second assignment to the first value.",
          "Subtract 3 from the current value 7.",
          "The new value is 7 - 3 = 4."},
         QuestionType::Output,
         "Assignment replaces 7 with 4."}},
       {{"int score = 3;", "score = 3",
         "Declaration creates an integer and gives it the value 3."},
        {"score = score + 2;", "score = 5",
         "Read score on the right, add 2, then replace its stored value."},
        {"printf(\"%d\\n\", score);", "score = 5 | output: 5",
         "Output reads the current value without changing it."}}},
      {"cpp-strings",
       "C++",
       "Strings & text",
       "Use std::string to store and combine text.",
       "std::string stores a sequence of characters and manages its own "
       "storage. Include <string> when using it. Two strings can be joined "
       "with +, and size() gives the number of stored characters.\n\nThe "
       "example starts with a name, builds a greeting and prints it. Spaces "
       "inside quotation marks are part of the text; keep them when predicting "
       "output.",
       "std::string name = \"Ada\";\nstd::string greeting = \"Hi \" + "
       "name;\nstd::cout << greeting << '\\n';",
       {{"cpp-str-p1",
         "What does the example print?",
         "Hi Ada",
         {},
         {"Combine the prefix and name.",
          "The prefix is \"Hi \" with a trailing space.",
          "\"Hi \" + \"Ada\" produces Hi Ada."},
         QuestionType::Output,
         "The space after Hi is part of the prefix."},
        {"cpp-str-p2",
         "Which member function returns the length of a std::string?",
         "size()",
         {"size()", "countText()", "lengthOf()"},
         {"It is called on the string object.",
          "The function is also used by several standard containers.",
          "name.size() returns the number of characters in name."},
         QuestionType::Choice,
         "size() reports the stored character count."},
        {"cpp-str-fresh",
         "A new example: std::string s = \"cat\"; s = s + \"s\"; What text is "
         "stored in s?",
         "cats",
         {},
         {"+ joins the two pieces of text.",
          "Append the one-character string s to cat.",
          "The result is cats, with four characters."},
         QuestionType::Output,
         "The new string contains cat followed by s."}},
       {}},
      {"c-pointers",
       "C",
       "Pointers & addresses",
       "Distinguish an address from the value it points to.",
       "A pointer stores the address of another object. &value obtains an "
       "address, while *pointer accesses the object at that address. The "
       "pointed-to object must still exist when you access it.\n\nHere p "
       "points to value. Assigning through *p changes value itself. The "
       "walkthrough uses symbolic addresses, because actual addresses differ "
       "between runs.",
       "int value = 10;\nint *p = &value;\n*p = 12;\nprintf(\"%d\\n\", value);",
       {{"c-ptr-p1",
         "Which expression obtains the address of value?",
         "&value",
         {"*value", "&value", "value + 1"},
         {"Use the address-of operator.",
          "The address-of operator is an ampersand.",
          "&value produces the address of value; *p follows a pointer."},
         QuestionType::Choice,
         "& obtains an object's address."},
        {"c-ptr-p2",
         "What does the example print?",
         "12",
         {},
         {"p points to value.",
          "Assigning to *p updates the pointed-to integer.",
          "*p = 12 changes value from 10 to 12, so the output is 12."},
         QuestionType::Output,
         "Writing through the pointer changes value."},
        {"c-ptr-fresh",
         "A new example: int x = 4; int *q = &x; *q = *q + 1; What is x?",
         "5",
         {},
         {"q refers to x.", "Read *q as x, then add 1.",
          "The assignment stores 4 + 1 through q, so x becomes 5."},
         QuestionType::Output,
         "Both x and *q refer to the same integer."}},
       {{"int value = 10;", "value = 10 | symbolic address: A",
         "The integer lives at a symbolic address A."},
        {"int *p = &value;", "value = 10 | p -> value (A)",
         "p stores the address of value, not a copy of 10."},
        {"*p = 12;", "value = 12 | p -> value (A)",
         "Dereferencing p accesses value. This assignment updates that "
         "integer."}}}};
}
std::vector<Comparison> comparisons() {
  return {{"Writing output", "printf(\"%d\\n\", score);",
           "std::cout << score << '\\n';",
           "C printf uses a format string. C++ streams insert values with <<. "
           "Include <stdio.h> in C and <iostream> in C++."},
          {"Working with strings",
           "char name[] = \"Ada\";\nprintf(\"%s\\n\", name);",
           "std::string name = \"Ada\";\nstd::cout << name << '\\n';",
           "A C character array includes a terminating zero. std::string "
           "manages its storage and offers member operations. Include <string> "
           "for std::string."},
          {"Changing a function argument",
           "void increment(int *n) {\n    *n = *n + 1;\n}\nincrement(&value);",
           "void increment(int& n) {\n    n = n + 1;\n}\nincrement(value);",
           "C passes values, including pointer values. A pointer parameter can "
           "modify its target. C++ also offers reference parameters that refer "
           "directly to the caller's object."}};
}
} // namespace cw
