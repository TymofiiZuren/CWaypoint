# Oleksandr's tasks — CWaypoint

## Proposed ownership

You own lesson content, answer evaluation, and AI assistance. Your teammate owns the raylib application, interface, and local progress. This is a proposed split for a first working version, not a record of implemented features.

## 1. Learning data and interfaces

- [ ] Agree with your teammate on C or C++ for implementation and the build setup before coding.
- [ ] Agree on the lesson structure: stable ID, language, title, explanation, example, question, answer choices, and feedback.
- [ ] Agree on the answer-result interface and how the UI requests and receives an AI hint.
- [ ] Supply one sample lesson early so the UI can be developed against real data.

Done when your teammate can display the sample lesson without embedding its content in UI code.

## 2. C and C++ lesson content

- [ ] Create introductory C lessons covering variables and types, conditions, loops, and functions.
- [ ] Create introductory C++ lessons covering input/output, references, strings, and basic classes.
- [ ] Give each lesson a short explanation, a code example, a question, and explanatory feedback.
- [ ] Check code examples with the agreed compiler and language standard.
- [ ] Make C and C++ differences explicit rather than presenting them as interchangeable languages.

Done when every lesson has a checked example and an answer with a clear explanation.

## 3. Answer evaluation

- [ ] Implement deterministic evaluation for quiz-style answers.
- [ ] Return correctness and useful feedback through the agreed interface.
- [ ] Handle unanswered questions and invalid choices consistently.
- [ ] Verify correct, incorrect, and invalid answers before connecting the evaluator to the UI.

Done when the same answer produces the same result and the UI can present feedback without duplicating evaluation logic.

## 4. AI learning assistance

- [ ] Agree on the AI provider, credential handling, networking approach, and any dependencies before implementation.
- [ ] Implement a hint request using the current lesson and learner's answer as context.
- [ ] Ask for explanations and hints suitable for beginners.
- [ ] Return clear outcomes for success, timeout, service failure, and missing configuration.
- [ ] Agree how requests run without blocking raylib's main loop, including cancellation or ignoring stale responses when the learner changes lessons.
- [ ] Verify the integration without exposing credentials in code, logs, or screenshots.

Done when a learner can receive a relevant hint and a failed request leaves the lesson usable. AI hints do not determine quiz correctness.

## Shared checkpoints

1. Before implementation: agree on lesson, answer-result, AI-hint, and progress interfaces with your teammate.
2. First integration: supply and complete one lesson through the raylib UI.
3. AI integration: verify success, unavailable service, and failed request states together.
4. Final demonstration: complete one C lesson and one C++ lesson, then restart and verify progress.

## Scope assumptions

The first version uses explanations, examples, and quiz-style questions. Compiling or running learner-submitted code is a separate scope decision. Your teammate implements local progress storage; supply stable lesson IDs so saved progress remains meaningful.
