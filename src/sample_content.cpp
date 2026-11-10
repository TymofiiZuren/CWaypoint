#include "core.hpp"
#include "lesson_adapter.hpp"
namespace cw {
std::vector<Lesson> sampleLessons() {
  return {
      adaptVariablesLesson(cwaypoint::variablesLesson()),
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
