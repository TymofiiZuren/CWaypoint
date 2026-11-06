# First lesson handoff

Include src/learning/lesson.hpp and link lesson.cpp. Call variablesLesson() to obtain the authored record. Call evaluate(exercise, answer) for correctness only. The controller must validate catalogue/exercise identity, own completion and durable assistance/attempt evidence, and decide when explanations are revealed. This fixture does not implement those controller responsibilities.

Predict-output comparison normalises CRLF and removes at most one final newline from each side; meaningful spaces and case remain significant. Fill-token comparison trims surrounding ASCII whitespace. Empty or whitespace-only input is invalid. Single-choice is provisional exact-ID comparison; a choices catalogue and unknown-option validation are required before using it in production exercises.

The Makefile is a standalone core harness, pending agreement on the application CMake setup. Run make test and make demo from the repository root. No Raylib or network dependency is needed for this first handoff.
