# Curriculum update 17 November 2026

The integrated macOS application now has eight lessons, 24 questions and 72 offline hint levels. This contribution adds C loops, C functions, C++ references and const, and C++ vectors. Each new lesson includes two practice questions, a separate fresh question, three hints per question, a walkthrough and a lab with three cases.

The current milestone is curriculum expansion and integration testing. The earlier application shell, notes, bookmarks, editor and bounded WebAssembly execution are in main. Remaining planned teaching topics are C program structure/output, validated input, arrays/strings, C++ input/output, overloading and classes/constructors. Live AI configuration and its failure/teaching-quality evaluation remain pending.

Validation performed by Codex

- All 15 CTest targets passed, including all eight labs through the sandbox helper.
- Four new lesson examples, twelve lab cases and output-based fresh examples were independently compiled and checked against authored answers.
- Reference UI journey passed at normal size, with saved practice and lab completion.
- Complexity gate passed, maximum function CCN 15.
- The compact reference journey fails with Settings actions overflow the profile panel. Native mouse/keyboard interaction on both teammates' Macs remains to be checked.

The course rail now scrolls independently so the expanded C catalogue fits smaller windows. C++ WASI compilation explicitly disables exceptions because vector library code otherwise fails to link with this runtime. Exception-handling exercises require further runtime work.

Review the new question wording and hint accuracy independently. Try a new lesson, fail its initial lab, edit toward the solution and run all three cases. Confirm completion and restart persistence. AI assistance generated the implementation and performed the recorded automated checks.
