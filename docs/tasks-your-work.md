# Your tasks — CWaypoint

## Proposed ownership

You own the raylib application, interface, and local learning progress. Oleksandr owns lesson content, answer evaluation, and AI assistance. This is a proposed split for a first working version, not a record of implemented features.

## 1. Application foundation

- [ ] Agree with Oleksandr on C or C++ for implementation and the build setup before coding.
- [ ] Set up the application entry point and raylib window.
- [ ] Add the main loop, input handling, and clean shutdown.
- [ ] Document how both teammates can build and run the app.

Done when both teammates can build from a fresh checkout, open the window, and close it cleanly.

## 2. Screens and navigation

- [ ] Build the home screen and lesson selection screen.
- [ ] Build a lesson screen showing explanation, example code, and a question.
- [ ] Add answer input, submission, feedback, and next-lesson navigation.
- [ ] Support readable text, scrolling where needed, and keyboard focus.

Done when a learner can complete a sample lesson without restarting the app.

## 3. Connect learning features

- [ ] Render lesson data supplied by Oleksandr rather than duplicating lesson text in the UI.
- [ ] Connect answer submission to Oleksandr's evaluator.
- [ ] Add an AI hint action with loading, success, unavailable, and error states.
- [ ] Keep the window responsive while an AI request is running.
- [ ] Keep lessons usable when AI is unavailable.

Done when correct and incorrect answers display appropriate feedback, and an AI failure does not interrupt the lesson.

## 4. Progress and application verification

- [ ] Track completed lessons locally.
- [ ] Restore progress after reopening the app.
- [ ] Handle missing or invalid progress data safely.
- [ ] Check the full journey: launch → choose lesson → answer → feedback → save → reopen.

Done when completion survives a restart and invalid saved data does not crash the application.

## Shared checkpoints

1. Before implementation: agree on lesson, answer-result, AI-hint, and progress interfaces with Oleksandr.
2. First integration: run one complete lesson using his content and evaluator.
3. AI integration: verify success, unavailable service, and failed request states together.
4. Final demonstration: complete one C lesson and one C++ lesson, then restart and verify progress.

## Scope assumptions

The first version uses explanations, examples, and quiz-style questions. Compiling or running learner-submitted code is a separate scope decision. AI provider, credentials, networking approach, and dependencies must be agreed before integration; never put API keys in source control or application logs.
