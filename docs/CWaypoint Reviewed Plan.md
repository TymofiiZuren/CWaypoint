<!-- /autoplan restore point: /Users/thomas/.gstack/projects/TymofiiZuren-CWaypoint/main-autoplan-restore-20261106-112344.md -->
# CWaypoint Reviewed Plan

Status: APPROVED FOR PLANNING
Updated: 6 November 2026

## Confirmed direction
C++17 and raylib desktop learning app for macOS, teaching C and C++ separately. Scope includes the complete baseline plus all five explicitly approved teaching additions. Accepted user decisions D9 through D15 remain binding; latest resolved D10 and D16 supersede earlier pending notes.

## Source record
/Users/thomas/.gstack/projects/TymofiiZuren-CWaypoint/ceo-plans/2026-11-04-cwaypoint.md
Original design and two Word work plans remain unchanged during intake.

## Confirmed premises
One local beginner learner; core learning works offline; deterministic grading; optional AI with authored fallback; two-person academic project rather than a hosted service. The approved content budget is 14 lessons, 42 questions and 126 authored hint levels. Deadline, provider, minimum OS and CPU compatibility remain implementation prerequisites.

## Decision Audit Trail
| ID | Phase | Decision | Classification | Principle | Rationale | Rejected |
|---|---|---|---|---|---|---|
| AP01 | Intake | Preserve approved scope and latest resolved decisions | Mechanical | Explicit | Existing user choices outrank stale pending notes | Restarting scope selection |
| AP02 | Intake | Consolidate into a readable Markdown plan | Mechanical | Pragmatic | Historical record contains superseded pending sections | Treating the history as current specifications |


## Phase 1 CEO review

### Premises and outcome
The user confirmed a two-person academic project, beginner audience, C++17, raylib, macOS, offline core, deterministic grading and optional AI. These are constraints, not hypotheses to reopen. The uncertain premise is that the combination improves understanding; completion counts alone do not establish that. Use the already approved fresh-question evidence and observe whether a learner can explain a new example; do not describe a prototype demonstration as proof of learning efficacy.

The problem framing is a guided route from reading to prediction to feedback and review. Broad curriculum coverage is secondary to making that route work across the accepted scope. Doing nothing leaves the two teammates with disconnected documents and no buildable learning experience. The competitive risk is duplicating tutorial pages without a clearer learning interaction; raylib walkthroughs and explicit assisted evidence are the chosen response, not a claim of market uniqueness.

### Existing code leverage and alternatives
| Subproblem | Existing evidence | Reuse decision |
|---|---|---|
| Navigation and controls | No application source | Build local raylib controls |
| Lesson content and evaluation | Proposed task lists only | One authored record format and evaluator |
| Persistence and AI | Design descriptions only | Implement behind narrow local interfaces |
| Team ownership | Two current Word work plans | Retain split; clarify handoffs |
| Learning policies | User decisions D9–D15 | Preserve as authoritative requirements |

| Approach | Effort and risk | Pros | Cons | Verdict |
|---|---|---|---|---|---|
| Minimal two-lesson prototype | Small; low integration risk | Fast vertical slice; proves contracts | Does not deliver approved full scope | Milestone only, not final scope |
| Complete local application with approved additions | Medium/large; content and integration risk | Meets user direction; no backend operations | Requires checked content and async/save work | Selected |
| Hosted execution and adaptive platform | Large; high security/operations risk | Richer free-form coding | New infrastructure and unapproved scope | Excluded |
Effort is relative, not a delivery promise. No credible calendar estimate exists before build, target-device and content-authoring experiments.

### Dream state and scope
```text
Design and task documents -> complete local learning journey -> independently applying concepts
                             with optional AI                 to unfamiliar problems
```
Twelve-month learning benefit is a hypothesis. Preserve all five additions, offline hint fallbacks and accepted recovery rules. Do not add accounts, code execution, telemetry hosting, CI/CD or release infrastructure. Small complete units are milestones through the scope, not silent feature cuts.

### Temporal interrogation
Hour 1 foundations: agree record IDs, revision rules and the build contract; OS version/architecture are documented prerequisites before claiming compatibility. Hours 2–3 core: define attempt identity, assistance timing and the exact evaluator rules. Hours 4–5 integration: test restart between attempt and result, stale hint delivery and reset while work is active. Hour 6 onward: verify content, text/focus layout, asset paths and shutdown; these are human sequencing landmarks, not a six-hour estimate. AI assistance may shorten coding, but compilation, device checks and teaching review still need evidence.

### 1 Architecture
Keep one desktop process and one owner of learning-state transitions. The UI gathers actions; the evaluator decides correctness; the controller applies immutable results; persistence records snapshots. Accepted walkthroughs and comparisons consume the same catalogue IDs and revisions as lessons, preventing duplicated completion rules.
```text
raylib screens -> controller -> catalogue + deterministic evaluator
                      |       -> authored walkthrough/comparison records
                      +-> learning evidence + review membership
                      +-> serialized progress store -> local primary/backup
                      +-> AI worker -> approved external endpoint
                           result queue -> controller context check
```
There is no earlier runtime dependency graph: before this work only documents exist. At 10x or 100x catalogue size, validation and text layout grow; no shared-service user load exists. Provider access, the catalogue and local saves are failure boundaries; offline authored hints and isolated content failures prevent an AI outage from becoming a whole-app outage. Source rollback cannot repair a changed save format, so readers must preserve unsupported files.

### 2 Error and recovery map
Use typed result categories for expected failures; these are planned C++ error values, not invented implemented exception classes. Never silently treat corruption, denied reads or unsupported versions as first launch. Each registry row specifies an owner, visible state and test; unexpected fatal initialization failure must produce a diagnostic and exit cleanly where possible rather than continue with invalid rendering resources.

| Codepath | Error category | Recovery owner and action | Learner sees | Planned test |
|---|---|---|---|---|
| Window/font initialization | AppInitFailed / AssetMissing | Bootstrap stops or uses an explicitly verified font fallback | Startup explanation | Missing font/init fixture |
| Catalogue load | CatalogueInvalid | Catalogue rejects invalid required records | Content unavailable | Duplicate IDs, unsupported type |
| Exercise selection/evaluation | ExerciseInvalid / AnswerMissing | Controller blocks affected exercise; evaluator returns Invalid | Specific question/input message | Unknown ID, empty answer |
| Walkthrough load/step | WalkthroughInvalid | Disable player, keep valid practice | Walkthrough unavailable | Empty steps, bad references |
| Comparison load | ComparisonInvalid | Disable affected comparison | Comparison unavailable | Missing side/language |
| Progress load | SaveMissing | Start fresh writable profile | Start learning | No file |
| Progress load | SaveCorrupt | Load compatible backup without overwrite; else temporary session | Recovered or Not saving | Corrupt primary/backup |
| Progress load | SaveUnsupported / SaveReadDenied | Preserve files; temporary session | Cannot load progress and recovery action | Newer version/denied read |
| Progress save/replace | SaveWriteFailed | Retain prior durable save; keep dirty state | Not saved and Retry | Full disk/replace failure |
| Second process | SaveLocked | Second instance does not write | Progress open elsewhere | Two-process fixture |
| AI configuration | AiNotConfigured | Authored hints remain available | AI not configured | No credential |
| AI transport | AiTimeout / AiNetworkFailed | No automatic retry; offer authored hints | AI unavailable | Delay/disconnect |
| AI authentication/limits | AiUnauthorized / AiRateLimited | Explicit recovery; no automatic retry | Check configuration or wait | Fake 401/429 |
| AI response | AiEmpty / AiMalformed / AiRefused | Reject result as unusable; authored fallback | Hint unavailable | Separate fixtures |
| AI response size | AiResponseTooLarge | Stop bounded receive; reject result | Hint unavailable | Limit plus one |
| AI late result | AiStale / AiCancelled | Reject mismatched context without changing lesson | Current lesson stays intact | Navigate/reset/answer change |

### 3 Security and privacy
The new inputs are authored records, learner answer text, local save bytes and provider responses. Set explicit size limits before parsing or rendering, validate IDs and enums, and never execute any of these inputs. Threat: credential leakage (medium likelihood/high impact); mitigation: process configuration for controlled developer use, no embedded shared key, no credentials in diagnostics or progress. Threat: unbounded provider or file data (medium/medium); mitigation: caps and typed validation errors. Threat: prompt injection (medium/medium); mitigation: AI returns display text only, never tool calls, grading commands or file access.
No remote public endpoint or multi-tenant account boundary is introduced. Writes are scoped to the current OS user and an application-controlled save path, not a path supplied by a lesson. Provider/networking dependency selection remains unresolved and must be reviewed before live integration; do not mark a dependency safe before it is chosen. Logs contain operation IDs, error categories and timings, not raw answers, prompts, credentials or response bodies.

### 4 Data and interaction edges
```text
Catalogue -> validate IDs/records -> render lesson
 missing/empty required data -> visible content error
 invalid optional walkthrough -> isolated unavailable player
 parser failure -> no partially trusted assessment

Answer -> validate -> deterministic evaluation -> learning reducer -> save -> feedback
 missing/empty -> Invalid; no mutation
 incorrect -> retain first-attempt evidence; add review item
 write error -> retain memory and prior file; Not saved
 stale exercise/revision -> reject before mutation

Hint action -> assistance transition -> request snapshot -> worker -> bounded result -> context check
 missing config/empty/refusal/error -> visible state + authored hint
 cancelled/stale -> never changes current lesson
 duplicate -> reuse active request or cached result, not new charge

Saved state -> version/record validation -> primary or compatible backup -> restored state
 missing -> new profile; corrupt/unsupported/denied -> preserve and recover/temporary
```
Double submit must be one accepted attempt. Back/Next invalidates request context and retains the agreed resume point. Empty review lists show a positive explanation, oversized catalogues scroll without drawing all text every frame, and reset invalidates pending AI and save work. The reviewed implementation must enforce these policies at the controller, not in independent screens.

### 5 Code quality
No source exists, so complexity and duplication cannot be measured. Avoid one class per screen plus one service per action as an assumed architecture; use small local records and functions until responsibilities justify separate units. Centralise evaluator normalization, evidence transitions and persistence ownership; intentional UI layout duplication is preferable to a generic form framework. Names should expose domain purpose (Attempt, AssistanceUsed, AssessmentRevision), not implementation mechanisms.

### 6 Tests and evidence
Required coverage spans UI actions, catalogue validation, evaluation, evidence state, walkthroughs, comparisons, persistence and AI. The highest-risk test is restart after help or an incorrect attempt but before a correct retry: assisted evidence must remain assisted. Hostile cases include repeated submit, malformed content, partial save replacement and a late hint after reset. Use fake provider and file-operation failure injection; external network and timing should not determine unit-test success. Detailed per-flow happy, failure and edge mappings will be produced in the engineering test artifact before this review can finish.

### 7 Performance
The three likely slow paths are AI transport, save replacement and text layout. The proposed 20-second request deadline is a bound, not measured latency; save and layout latency are unknown until implementation. Keep drawing on the main thread, network off it, cache layout by content/size rather than by frame, and bound retained hints. No database queries, indexes, N+1 queries or connection pool are present in this design; introducing those would add unnecessary architecture.

### 8 Diagnostics and debugging
Local diagnostics should identify operation, category, synthetic/context ID and timing, with rotation or an explicit size cap. A learner sees actionable messages for save, content and AI failures; they do not need a developer console. A developer reproduces failures with fixtures and an isolated data directory, never by requesting real learner logs containing answers. No hosted dashboard or alerting system is appropriate to this approved desktop scope; the build's local check report is the operational evidence.

### 9 Distribution and rollback
This is an academic macOS application, not a hosted deployment. Build a runnable local artifact with its assets, test it from another working directory, and document supported OS/architecture only after tests on actual devices. No CI/CD or automatic release ceremony is authorised. Validate a copy of progress with a new build before replacing a demo build; rollback opens the prior build only with compatible state, otherwise preserves the file and enters temporary mode.
```text
build -> core tests -> asset/path check -> teammate device demo -> accepted local build
failure -> keep previous artifact and saved-state backup -> diagnose -> repeat
rollback -> old reader supports save? -- yes -> open preserved compatible copy
                                  -- no  -> preserve state; temporary learning
```

### 10 Long-term trajectory
The biggest debt is content verification, not missing services. Use stable IDs and assessment-specific revision changes so editorial text fixes do not unnecessarily invalidate learning evidence. This architecture is mostly reversible (4/5) before data ships; save migrations become less reversible afterward. Both teammates need executable contract examples and ownership notes so UI, grading and AI are not knowledge silos. All five additions stay in scope; optional future platform capabilities do not become hidden prerequisites.

### 11 Design and UX
Order the experience around one primary action: start/continue a lesson, read and predict, then see feedback. Do not make the learner configure AI before learning, and do not show a first-launch empty statistics dashboard. The approved independent/assisted labels should be descriptive, never punitive. The new walkthrough and comparison views need keyboard and large-text specifications, and each state must include local recovery actions; phase 2 will make those concrete.

### Failure modes registry
All checks below are planned, not implemented. “Handled” means specified in the plan, not observed at runtime.
| Path | Failure | Handled in plan | Planned test | User sees | Diagnostic |
|---|---|---|---|---|---|
| Content | Missing/invalid question | Yes | catalogue invalid fixture | Question unavailable | Content ID/category |
| Visual records | Bad step/comparison | Yes | reference/empty fixtures | Affected view unavailable | Record ID/category |
| Evaluation | Duplicate/stale/empty submission | Yes | reducer/evaluator | Stable state or input message | Event/category |
| Evidence | Help lost across restart | Gap found; engineering phase must resolve | persistence restart test | Incorrect independent claim if unresolved | State revision/category |
| Save | Corrupt/denied/unsupported | Yes | injected failures | Recovery/temporary | Save category |
| Save | Reset races pending write | Gap found; engineering phase must resolve | reset/write barrier | Old state could reappear | Generation/category |
| AI | Timeout/401/429/refusal/empty/oversize | Yes | fake-provider matrix | Error plus authored hints | Request ID/category |
| AI | Navigate away or reset | Yes | late result | No stale hint applied | Stale category |
| Startup | Assets/init failure | Yes | missing asset | Startup explanation | Initialization category |
No plan-silent unhandled failure is accepted. Two explicit integration gaps carry forward to engineering; no runtime clearance is claimed.

### CEO completion summary
Mode: SELECTIVE EXPANSION, preserving the already approved scope. Eleven sections evaluated; 17 named error-category groups mapped; two state/persistence integration gaps carry to engineering. Required architecture, shadow paths, rollback/distribution diagrams, scope exclusions, leverage map and dream delta are written. No new feature expansion or cut is auto-approved; unresolved provider, compatibility and scheduling details remain explicit prerequisites. Independent voice results are appended before phase transition. This is a planning review, not shipping clearance.

### CEO acceptance amendments
The final implementation must deliver all five additions: one variables and one pointers walkthrough; one fresh check per lesson; saved mistake review; output/strings/function-arguments comparisons; three AI hint levels with three authored levels per exercise. Proposed finite curriculum is the existing 8 C topics and 6 C++ topics: program structure/output, variables/types, validated input, conditions, loops, functions, arrays/strings, pointers; and C++ input/output, strings, references/const, vectors, overloading, classes/constructors. This inventory was approved by the user on 6 November 2026; it is not a delivery-date promise.
For that inventory, plan two practice exercises plus one fresh check per lesson (42 questions, 126 authored hint levels), covering all three existing exercise types across the catalogue. This is a concrete authoring budget to review, not evidence of throughput. Oleksandr authors records and verifies compiled examples; the user independently checks expected answers and hint clarity, owns integrated UI/evidence/save behaviour, and both accept the final journey. If the academic brief requires different inventory, revise before content production.
The release claim is supported practice and accurate first-attempt evidence. Independent success on one question is not a claim of retained mastery; label it Solved independently on first attempt. No new learner study, external contact or data collection is authorised.

### Decision audit additions CEO
| ID | Phase | Decision | Classification | Principle | Rationale | Rejected |
|---|---|---|---|---|---|---|
| AP03 | CEO | Treat premises as confirmed by current user A | Mechanical | Explicit | Human gate passed | Re-asking approved premises |
| AP04 | CEO | Preserve complete approved feature set | Mechanical | Completeness | All five additions remain required | Cutting features for smaller MVP |
| AP05 | CEO | Recommend 14 lessons with two practice questions and one fresh check each | Taste | Pragmatic | Makes existing proposed topics finite and authoring measurable | Unbounded curriculum / arbitrary smaller subset |
| AP06 | CEO | Name authorship and independent verification handoffs | Mechanical | Explicit | Prevents correctness and integration becoming ownerless | Module-only ownership |
| AP07 | CEO | Limit claims to recorded immediate evidence | Mechanical | Explicit | No retention study exists | Claiming mastery from one check |

### CEO outside voices and consensus
Claude CLI was attempted but returned Not logged in; it supplied no review. Do not attribute the independent subagent to Claude: it is an additional independent agent using the host model. Codex CLI returned a five-finding strategic review; the independent agent returned six findings. Reports are retained in .scratch/autoplan/ceo-codex.md and ceo-independent.md. Both stress a finite curriculum, honest evidence claims and integrated delivery. No model pair recommended changing the user's chosen platform or removing approved additions.
| Dimension | Claude | Codex | Independent agent | Required dual-model consensus |
|---|---|---|---|---|
| Premises | Unavailable | Conditional | Conditional | N/A |
| Problem | Unavailable | Conditional | Coherent, evidence gap | N/A |
| Scope | Unavailable | Inventory needed | Inventory needed | N/A |
| Alternatives | Unavailable | Clarify baseline | End-to-end slices preferred | N/A |
| Competitive risks | Unavailable | Unvalidated | Non-blocking academic context | N/A |
| Trajectory | Unavailable | Conditional | Content maintenance risk | N/A |
CEO phase output complete as a document review with degraded model diversity. No cross-model CONFIRMED claims are made. Curriculum sizing remains a final taste decision; provider and compatibility remain preimplementation prerequisites. Proceed to design after this persisted completion record.
## Phase 2 Design review

### Scope and existing system
UI scope is present: screens, navigation, code examples, answers, walkthroughs, comparisons and hints. No DESIGN.md or implemented primitives exist. Initial design-spec completeness is assessed at 6/10: the original design names screens and broad typography but not the complete state matrix, focus rules or enlarged-text layouts. This phase specifies them; visual quality remains unverified because the designer command failed for missing credentials. No mockups are approved or claimed generated.

### Pass 1 Information architecture
Before 6/10; after specification 8/10, pending usability validation. The lesson workspace, not a metrics dashboard, is the main visual anchor. First launch shows CWaypoint, a short C/C++ purpose statement, and Start learning; returning launch gives Continue priority with track/title and an unambiguous secondary Choose lesson action. Lesson pages put objective, current code/question, and the primary action ahead of optional assistance or completion counts.
```text
Home -> Choose track -> Lesson list -> Read -> Practice -> Fresh check -> Summary
  |                                      |         |         |
  +-> Continue --------------------------+         +-> graduated help
  +-> Progress -> Review mistakes -----------------+-> retry
  +-> Settings
Lesson -> authored walkthrough / paired comparison -> return to same lesson position
```
Three visible priorities are current concept, current task, and next action. Use a persistent Back to lessons control, a topic/step label, and one primary Submit/Retry/Next action whose label reflects state. No animation, comparison or hint is allowed to reset an answer draft merely by opening a panel.

### Pass 2 Interaction state coverage
Before 6/10; after 9/10 as a written specification, not rendered evidence. Every state names what the learner sees and can do. No spinner is the only explanation for a blocked action; no raw provider error or internal path is shown to learners.
| Feature | Loading | Empty | Error | Success | Partial |
|---|---|---|---|---|---|
| Home/restore | Restoring progress; Start temporarily disabled | Start your first lesson | Cannot load progress; recovery/temporary action | Continue named lesson | Temporary session; Not saving banner |
| Catalogue | Loading lessons | No valid lessons available; Back | Content unavailable with affected topic | Ordered rows with objective/status | Invalid lesson rows unavailable; others usable |
| Exercise | Preparing question | Missing answer: Enter an answer | Question unavailable or specific input problem | Correct/incorrect text with explanation | Draft retained; prior attempt evidence labelled |
| Walkthrough | Preparing example | No steps; walkthrough unavailable | Player unavailable; practice stays open | Code line and variable state with step count | Pause/step position retained while panel open |
| Comparison | Preparing comparison | Comparison unavailable | Affected comparison unavailable | Labelled C and C++ code/explanation | Other comparisons remain usable |
| Fresh check | Preparing new question | Enter answer | Question unavailable | Independent first attempt or solved with practice | Incorrect first attempt preserved during retry |
| Mistake review | Loading review items | No questions to review; Choose lesson | Item no longer available | Correct retry removes item | Stale-revision items flagged, not graded blindly |
| AI hint | Requesting level name; cancel/help available | Empty response: AI hint unavailable | Clear timeout/config/service message; authored option | Label AI generated; context and level visible | Existing hint remains while explicit regeneration runs |
| Local hint | Immediate local content | Missing authored hint is content error | Keep practice usable | Label Authored hint and level | Available levels only; missing required content fails acceptance |
| Save/reset | Saving / resetting | New profile | Not saved with Retry; preserve files | Saved or reset complete | In-memory work retained; temporary mode explicit |

### Pass 3 Learner journey and emotional arc
Before 5/10; after 8/10. First-time confusion should be reduced by one obvious first action, a short objective and a runnable-looking authored example, not promotional copy. During errors the app names the problem without judging ability. First-attempt evidence is descriptive; never label assisted learning as failure or erase its legitimate practice completion.
| Step | Learner action | Likely feeling | Specific support |
|---|---|---|---|
| 1 | Starts without AI setup | Uncertain | Direct first lesson and offline promise |
| 2 | Reads and predicts | Curious | Objective plus short code example |
| 3 | Gets answer wrong | Frustrated | Explain misconception; Retry and Hint |
| 4 | Uses graduated help | Supported | Level names and source labels |
| 5 | Tries fresh question | Cautious | Explain first-attempt evidence before answering |
| 6 | Restarts and reviews | Reassured | Resume title and preserved status |
The five-second goal is orientation; the five-minute goal is a completed practice loop. Long-term reflective value is accurately recorded progress and understandable revision changes, not a badge that overstates mastery. Learner emotions are design hypotheses, not observed research results.

### Pass 4 Specific interface rather than generic decoration
Before 5/10; after 8/10. Classify as application UI, not a marketing page. The screen is a calm workspace with a left topic list, central code/question area and optional right assistance pane; do not construct a decorative dashboard-card mosaic. Use restrained blue primary actions, text labels and a single code/variable area as the visual anchor; shadows, gradients and ornamental animation are unnecessary.
Walkthrough motion must explain a state change and allow pause, next, previous and restart; default to explicit stepping rather than autonomous playback. The comparison view has two language-labelled columns with a shared explanation; it becomes vertically stacked at large text sizes. No decorative icons, hero image, card carousel or landing-page pattern is required for the approved app.

### Pass 5 Design vocabulary and tokens
Before 4/10; after 8/10 pending visual checks. Define proposed tokens in this plan rather than falsely claiming an existing design system: background #F7F8FA, main surface #FFFFFF, primary text #17212B, secondary text #465463, action #165DB5, focus #0B4F9C, error #A61B1B. Validate actual foreground/background combinations for at least 4.5:1 body-text contrast; numeric colours are proposals, not measured compliance. Use an 8 px layout scale, 16 logical-pixel body and code minimum, 24 section and 32 screen headings.
Use a separately approved redistributable body font and code font; do not silently bundle an OS font or add a production dependency. Typeface selection is final-gate taste and licence verification work. Reusable controls are Button, Topic row, Choice, Labelled text input, Code block, Status message and Step control; each owns its surface, padding, focus and typography. UI tests check semantic states rather than consumers overriding visual internals.

### Pass 6 Desktop resizing and accessibility
Before 5/10; after 8/10, with a remaining native-accessibility feasibility risk. At 1280x800 logical pixels, use approximately 216 px topic navigation, a flexible main column and 280 px hint pane, all within shared padding/gaps. Below roughly 1120 px available width, collapse topic navigation into a labelled selector and put help below the question; at the 960x640 proposed minimum, use one main reading column and scrolling. At enlarged text, prefer the same single-column layout rather than shrinking text or clipping controls.
Keyboard focus follows reading order; Tab/Shift-Tab traverse controls, Enter/Space activate buttons and choices, arrow keys operate the selected choice/step control, Escape closes the current transient pane and restores its invoking control. Text inputs retain native editing expectations to the extent implemented; shortcuts never steal typing. Require visible labels, 44 logical-pixel targets for primary controls, colour-independent correctness, and focus restoration after errors. VoiceOver support for a custom raylib renderer is unverified and may require a platform bridge; surface that feasibility decision at the final gate rather than claiming conformance.

### Pass 7 Remaining design choices
Before 4/10; after 8/10 for specification, with taste/feasibility open. Light workspace versus dark code theme is a taste choice; recommend the specified light shell with high-contrast code and postpone theme switching because it is not approved scope. Typography assets and VoiceOver feasibility need validation before design acceptance. Fixed layout numbers are starting constraints to test with real content, not pixel-perfect mockups; no generated visual board or user-approved screenshot exists.
The original Word documents remain historical snapshots. Once the plan is approved, align task lists and design documents rather than implementing stale UI states from an older file. Mobile layouts remain outside scope; desktop resize and enlarged text are required, not a mobile redesign.

### Design litmus assessment
| Check | Primary assessment | Evidence limitation |
|---|---|---|
| Brand identifiable | Specified | No first-screen render |
| Strong visual anchor | Code/question workspace | No mockup |
| Headline scanning | Named concept and action hierarchy | Needs real content |
| One job per section | Reading/practice/help separation | Needs navigation test |
| Cards necessary | Only selectable interactions, no mosaics | No visual QA |
| Motion useful | Step-based explanation only | No live player |
| No decoration dependency | Typography/spacing hierarchy | No render |
No hard visual rejection can be verified without images. No claim of 10/10 or visual clearance is justified.

### Design completion summary
All seven passes evaluated. Scores are reviewer judgments of written specification only; mean rises from 5.0 to 8.1 after state, focus and layout additions. Mockup generation was attempted and failed due to missing API credentials; Claude voice is unavailable due to authentication; independent and Codex outputs are appended next. Required screen graph, state matrix, storyboard, tokens, reuse map and exclusions are present. Remaining work is implementation, rendered visual checks, font licensing and native accessibility feasibility; these are not silently waived.

### Decision audit additions Design
| ID | Phase | Decision | Classification | Principle | Rationale | Rejected |
|---|---|---|---|---|---|---|
| AP08 | Design | Code/question workspace as main anchor | Mechanical | Explicit | Learner needs the current task first | Metrics dashboard first |
| AP09 | Design | Define all loading/empty/error/partial states | Mechanical | Completeness | Avoids silent blocks and blank panes | Happy-path-only screens |
| AP10 | Design | Collapse side panes at narrow width or large text | Mechanical | Pragmatic | Preserve legibility without clipping | Shrinking body text |
| AP11 | Design | Recommend light shell and licensed font assets | Taste | Explicit | Coherent visual starting point | Unspecified theme and font copying |
| AP12 | Design | Flag VoiceOver bridge feasibility | Taste/feasibility | Completeness | Custom drawing does not prove accessibility | Claiming support from keyboard controls |

### Design synthesis and amended contracts
Codex supplied five findings and the independent agent supplied six. Claude was unavailable, so the seven-check Claude/Codex consensus is N/A, not confirmed; independent evidence is supplementary. Both available reviews identified evidence wording, focus/error priority and accessibility gaps. Resolve the first-launch inconsistency: Start learning opens track choice, then the ordered lesson list; no automatic first-lesson jump. Continue restores the saved valid lesson/step; if unavailable, explain and return to the relevant catalogue.
Use exactly three evidence labels across all consumers: Not yet checked; Solved independently on first attempt; Solved with practice. An incorrect first answer before eventual success shows Needs more practice and retains the incorrect-first fact, not an independence label. Practice completion and this evidence label are separate fields; review-list removal cannot rewrite either historical first attempt or assistance.
Submit on an incorrect answer becomes Retry; Retry permits a new answer. Correct practice permits Next; all required practice questions solved makes the fresh check available. Reading can be revisited without erasing draft or attempt facts. Reveal/Hint must explicitly state it provides assistance; the controller records assistance before showing content, with engineering defining the durable barrier.
Message priority: a blocking assessment error belongs at the question; save/temporary status persists at the workspace edge; hint/player status stays local to its pane. Correct-answer feedback never hides Not saved. Save Retry only retries persistence, never resubmits an answer. On failure, focus moves only to an actionable blocking message or remains on the triggering control; background status changes must not steal focus.
Define answer editing explicitly: selection, insertion, Backspace/Delete, left/right movement, Home/End equivalents, copy/paste and UTF-8 bounds, with no shortcut interception inside a field. Long authored code preserves indentation and has keyboard-accessible horizontal scrolling. Native accessibility remains a feasibility gate; no VoiceOver clearance or compliance claim is given.
| Litmus | Claude | Codex | Independent agent | Consensus |
|---|---|---|---|---|
| Brand | N/A | Needs title/track treatment | Underspecified | N/A |
| Anchor | N/A | Well directed | Conditional | N/A |
| Scanning | N/A | Needs real content | Partial | N/A |
| One job | N/A | Specify coexisting states | Conditional | N/A |
| Cards | N/A | Restrained | Unproven | N/A |
| Motion | N/A | Purposeful intent | Conditional | N/A |
| Decoration | N/A | Well directed | Conditional | N/A |
Design phase complete with written fixes, mockup/auth degradation and explicit accessibility limitation. Do not treat numerical specification scores as visual validation.
## Phase 3 Engineering review

### Scope challenge and evidence
Inspected README, two historical task Markdown files, the confirmed decision history, current work-plan DOCX content and consolidated review plan. There are no src, tests, CMakeLists.txt, installed package manifest or existing runtime to inspect. Therefore findings are specification gaps, not demonstrated code bugs; confidence in those gaps is 7–8/10, while runtime correctness is unverified. More than eight eventual source/test files is plausible but not itself a reason to remove approved features: retain local modules, avoid a service hierarchy and implement one complete slice first.
Source lookup confirmed raylib exposes the expected drawing/font/frame controls and CMake can register tests with CTest. This does not establish that a particular raylib release, dependency arrangement or OS architecture is installed or compatible. C++17 worker cancellation must be designed explicitly; do not assume C++20 stop-token facilities or a detached worker can provide safe lifetime management.

### 1 Architecture and contracts
Proposed ownership is explicit: the user owns app/UI/controller/progress and Oleksandr owns catalogue/evaluator/AI. Both agree the data records before parallel implementation and independently check authored outputs. Catalogue data contains stable lesson/exercise IDs, content version, assessment revision, track, objective, ordered explanatory/code blocks, required exercise IDs and optional walkthrough/comparison references. Each exercise contains kind, prompt, choice IDs or explicit accepted answers, feedback and three authored hint levels; transfer questions have separate IDs.
```text
Input actions -> AppController (single state owner)
                   +-> Catalogue validation -> immutable teaching records
                   +-> Evaluator(answer, exercise) -> Correct / Incorrect / Invalid
                   +-> Evidence transition -> serialized durable write -> apply display
                   +-> Walkthrough state -> code/variables (no execution engine)
                   +-> Hint context -> one AI worker -> result mailbox -> context check
                                            |
                                  approved HTTPS provider only
```
Keep std::string/std::vector/enum records and small functions. Do not add a general event bus, plugin system, backend or state-management dependency. Expected errors return typed results; resource-owning objects clean up through RAII, and worker boundary failures are converted into a named result rather than escaping a thread. Provider, HTTP/JSON approach and dependency versions remain explicit choices before implementation of the real adapter; fake provider use is not a substitute for delivering the approved live optional AI milestone.

#### Evidence persistence barrier
A previous plan gap allowed restart after seeing help or after a wrong answer to restore pristine first-attempt state. Resolve with a durable assessment-attempt record before revealing a fresh question and before exposing any assistance for that assessment. Store assessment ID/revision, attempt sequence, question-issued fact, first submitted answer correctness when available, assistance-used, and completion/evidence outcome; raw answer text need not be persisted. Question-issued and first-submitted are distinct facts: opening then returning must not invent an incorrect submission.
Before accepting a submission or displaying assessment help, write the proposed transition successfully, then apply its visible outcome. If a crash occurs after durable commit but before display, replay the committed result on restart without regrading or incrementing attempts. Duplicate UI events carry the same event ID; only explicit Retry creates a new sequence. Help-request admission is conservatively recorded before contacting the provider; even a failed request preserves assisted context, preventing restart from creating independent evidence after attempted help. This conservative interpretation is surfaced as a final taste choice because failed requests might not reveal information.
If the required durable write fails, keep the prior durable state and show Not saved. Practice can continue in memory with an explicit temporary/unsaved status. Do not expose a new fresh assessment in a temporary session when it cannot record issuance; explain that practice and hints remain available, and fresh checking resumes after saving is restored. A previously issued assessment whose durable evidence is available can be revisited without inventing pristine evidence. This bounds the promise: independent evidence is claimed only for successfully recorded transitions, not for arbitrary power-loss durability.

#### Reset and writer ordering
All save snapshots carry a monotonically increasing profile generation and sequence. A single serialized writer owns the actual replace operation, and each result identifies its generation. Reset increments the controller generation, invalidates hint contexts, cancels queued old-generation saves, and waits for any already-running replace operation before durably writing the empty new-generation profile. Only after that write succeeds does the UI show Reset complete; failures retain visible state and actionable retry without claiming deletion succeeded. A late old-generation completion cannot overwrite the new save or change current UI status. Do not solve this solely by ignoring callbacks: the disk write itself must be ordered.
A second app instance uses a per-profile single-writer lock or read-only/temporary mode; lock acquisition must not delete another instance's files. First launch alone creates a new writable profile. Corrupt, unsupported and unreadable files remain distinct; no automatic downgrade or overwrite. Recovery loads backup into memory, preserves damaged primary, and requires an explicit recovery commit before overwriting an occupied invalid path. Reset confirmation names what is lost, including temporary session work.

#### Worker lifetime and bounds
Use one worker and bounded request/result storage; the main thread alone touches raylib and app state. Each request snapshots profile generation, lesson/exercise revisions, answer revision, hint level, configuration generation and a unique request ID. Success requires all context fields to match on delivery. Timeout, cancellation, navigate-away, answer edit, reset and configuration change invalidate the request; the UI can use authored hints while invalidation proceeds.
The selected transport must offer a real total deadline covering connect, TLS, upload and response, plus cancellation or a verified bound. Proposed deadline remains 20 seconds; shutdown cancels, joins the worker, and never destroys context while it can still be used. std::thread alone does not cancel blocking I/O; no detached worker, use-after-free risk or promise of instantaneous exit is accepted. The provider prototype must demonstrate cancellation and deadline behaviour before real AI integration is declared ready.
Proposed resource limits for validation: 512 lessons; 32 exercises/lesson; 256 walkthrough steps/example; 64 KiB per explanatory/code block; 8 KiB learner answer; 2 MiB save; 64 KiB provider envelope and 4 KiB extracted hint. These are testable conservative bounds, not measured needs. Bound both envelope receive and decoded display text; reject a truncated successful-looking answer rather than silently clipping it. Retain at most three successful hints for the active exercise context; leaving it may discard memory-only responses without disk history.

### 2 Code quality and consistency
No source-level DRY or branch-complexity defect can be asserted. The current documents do show stale C/C++ undecided text in the older task Markdown and original design; the new consolidated plan's resolved C++17/macOS statements take precedence. Do not silently edit the previously delivered Word files during review; list their alignment as a documentation follow-up after approval.
Evaluator contracts are exact: SingleChoice uses stable choice IDs, never displayed order; PredictOutput maps CRLF to LF and ignores at most one final newline while preserving meaningful spaces/case; FillToken trims surrounding whitespace and compares against explicit allowed tokens. Empty input is Invalid, not Incorrect; bad authored data blocks its exercise. Neither AI nor UI reimplements accepted-answer semantics. Record schemaVersion separately from assessmentRevision; editorial clarification does not automatically invalidate a completed assessment.
Walkthrough records are pure authored snapshots with stable variable identifiers and a validated step index; no C parser or execution simulator is implied. C/C++ comparison records share a concept ID but keep language labels and examples separate. Missing optional visuals are isolated at runtime, while required visuals and all authored hint levels must pass the release inventory validator. Code comments should keep the reset/evidence and worker lifetime diagrams alongside their owning modules once implemented; diagram maintenance is part of boundary changes.

### 3 Test review and coverage map
No test framework or existing tests were found, and no executable coverage percentage can be claimed. Recommend a small standard-library C++ test executable registered with CTest, avoiding a new production or test dependency until a real need is shown. Headless evaluator, validation and persistence tests should not require raylib, a window or an API credential. Planned commands are contracts to implement, not commands run in this review: cmake -S . -B build; cmake --build build; ctest --test-dir build --output-on-failure.
```text
NEW UX [all PLANNED, none tested]
 Home/start/continue -> track/list -> lesson -> submit/retry/reveal -> fresh check/summary [E2E]
 walkthrough previous/next/restart; comparisons; keyboard/large text [manual/system]
 three-level authored/AI hints -> loading/error/reuse/regenerate [integration + EVAL]
 progress/review mistakes/reset/temporary/recovery [integration + E2E]

NEW DATA / CODE PATHS
 catalogue -> valid / missing / duplicate / unsupported / invalid optional record [unit]
 evaluator -> three kinds x correct / incorrect / empty / malformed / boundaries [unit]
 evidence -> first / retry / assisted / crash replay / duplicate / revision [unit+integration]
 save -> missing / good / backup / corrupt / denied / unsupported / locked [integration]
 write -> commit / fail / interrupt / old generation / reset barrier [integration]
 walkthrough -> bounds / unknown variable / valid static steps [unit+system]
 hint -> context / cancellation / timeout / 401 / 429 / refusal / malformed / empty / oversize [fake integration]

ASYNC
 AI worker -> finite lifecycle -> mailbox -> stale rejection -> cancellation/join [integration]
 save writer -> ordered generation/sequence -> reset -> no old overwrite [integration]

EXTERNAL
 transport prototype -> real TLS/deadline/credential test (synthetic context only) [integration]
 selected prompt/model -> six exercises x three levels + malformed/failure fixtures [EVAL]
```
The separate test artifact specifies every row's happy, failure and edge assertions. Prioritise many pure unit cases, fewer filesystem/provider integration cases and a small full UI journey set; do not inflate UI snapshots as a substitute for state contracts. Use deterministic IDs and a controllable fake clock/barriers rather than sleep-based races. No tests generated or run because no application implementation was requested.

### 4 Performance review
No database or shared server exists: N+1, indexes and DB pool pressure are not present. A 60 FPS navigation target means roughly a 16.7 ms frame budget; it is a proposed reference-device target, not an observed percentile. Measure layout separately from rendering and reuse layout until content, available width or text scale changes; avoid parsing catalogue data or allocating font textures each frame.
Save latency is unknown, so durable barriers display a Saving state and keep drawing responsive using the single writer. AI work is one request with a total deadline and bounded envelope; prevent a backlog of hidden retries. At 10x/100x proposed content, exercise caps and file limits fail clearly, rather than claiming unlimited scaling. Record actual build/load/frame/save/shutdown measurements on the supported macOS machines before accepting compatibility or latency promises.

### Engineering failure-mode resolution
The CEO gaps are now specified: attempt/assistance state is durably admitted before protected display/result transitions; reset orders actual disk writes by generation and writer barrier. Residual risk is implementation violating those protocols, covered by crash/reorder tests; planned handling is not runtime proof. Provider/networking choices, target OS/architectures, font licences and VoiceOver feasibility remain explicit prerequisites; none are papered over as working integrations.
The full error registry in phase 1 remains authoritative with additional categories AttemptCommitFailed, SaveLocked, ResetWriteFailed, CatalogueLimitExceeded and TransportDeadlineUnsupported. Each has a planned test and visible outcome: unsaved/temporary state, read-only instance, reset not completed, content unavailable, or live AI not enabled respectively. No planned silent unhandled path is accepted; no implemented critical-gap count is asserted.

### Engineering completion summary
Four sections evaluated; two high-risk specification races addressed; named error/result contracts and exact evaluator semantics added; planned resource caps added. Architecture and per-flow test diagrams are written; test artifact is produced separately. All implementation paths currently have no executable tests; this is expected because implementation is out of scope, and it prevents a runtime-clear verdict. Proposed commands and dependency choices need implementation before validation. Independent voice results and final fixes follow before DX.

### Decision audit additions Engineering
| ID | Phase | Decision | Classification | Principle | Rationale | Rejected |
|---|---|---|---|---|---|---|
| AP13 | Eng | Durable evidence barriers and replay IDs | Mechanical | Completeness | Restart must not restore pristine first-attempt state | Save only completion |
| AP14 | Eng | Serialize reset with actual old writes | Mechanical | Explicit | Ignoring callbacks does not prevent disk overwrite | UI-only reset guard |
| AP15 | Eng | Require bounded/cancellable transport before live AI | Mechanical | Completeness | C++17 thread join cannot cancel I/O | Detached thread/indefinite exit |
| AP16 | Eng | Headless standard-library tests via proposed CTest | Mechanical | Pragmatic | No existing test framework; no added dependency needed | GUI-only tests |
| AP17 | Eng | Validate envelope and hint bounds separately | Mechanical | Completeness | Transport wrappers may exceed hint length | Unbounded receive or silent truncation |
| AP18 | Eng | Conservatively count admitted hint requests as assistance | Taste | Explicit | Durable admission prevents ambiguous replay | Count only visible successful help |
| AP19 | Eng | Require successful issuance record for a new fresh check | Mechanical | Completeness | Temporary-mode exposure cannot support durable evidence | Expose pristine assessment without persistence |

### Provider boundary security specification
Only the approved provider adapter receives the credential; the generic request contains question text, relevant authored code, an optional learner answer and requested hint level. Local profile IDs, save paths, attempt history and unrelated files are not transmitted. Before first external request, disclose provider and fields and require explicit AI enablement plus a hint action. The adapter verifies TLS and disallows credential-bearing redirects to another origin; the endpoint is configuration owned by the application, not supplied by model output or lesson content.
Provider-specific request/response schema, refusal mapping, HTTP library, JSON parsing and credential source must be chosen and approved before live implementation. This review does not approve any new production dependency or make provider retention/cost claims. Use synthetic requests for the prototype and eval; no real learner payload is required to verify lifecycle handling.

### Canonical assessment transition table
First-attempt facts and completion are separate, monotonic per assessment revision; wording is presentation, not a replacement for the facts.
| Current facts | Event | Next facts and display |
|---|---|---|
| No issuance | Open fresh check | Durably record issuance; then display question; Not yet checked |
| Issued, no submission/help | Empty/invalid answer | No accepted submission; input message; Not yet checked |
| Issued, no submission/help | Correct first submission | Commit firstCorrect=true, solved=true; Solved independently on first attempt |
| Issued, no submission/help | Incorrect first submission | Commit firstCorrect=false; Needs more practice; review item added |
| Any unsolved | Hint/reveal admitted | Commit assisted=true before content/external request; no independent outcome allowed |
| Incorrect first or assisted | Correct explicit retry | Commit solved=true; Solved with practice; remove review item |
| Solved | Duplicate event | Idempotent; no attempt increment or historical change |
| Any | Editorial-only change | Keep assessment revision and evidence |
| Any | Changed assessment revision | Preserve old record separately; new check is Not yet checked |
| Any | Unsupported or absent record | Do not grade; show unavailable/revision explanation |
The existing practice-completion decision is preserved; all required practice questions solved marks practice complete. Correctness and first-attempt evidence attach to individual assessments, not a broad mastery claim.

### Test artifact
Detailed required coverage: docs/CWaypoint Test Plan.md and the local gstack copy thomas-main-eng-review-test-plan-20261106.md. Twenty-two test families each specify happy, failure and edge assertions; none are executable or passed yet. No regression against existing runtime is claimed because the repository has no runtime.

### Engineering source checks
- raylib official API cheatsheet: https://www.raylib.com/cheatsheet/cheatsheet.html
- CMake official CTest tutorial: https://cmake.org/cmake/help/latest/guide/tutorial/Testing%20and%20CTest.html
These support using existing rendering/font controls and CTest registration rather than building replacement infrastructure. Installed versions and real platform behaviour remain unverified.

### Engineering outside review and final contract amendments
The independent agent supplied six findings; Codex supplied six findings. Claude remains unavailable, so architecture/tests/performance/security/errors/distribution dual-model consensus is N/A across all six dimensions. Both available reports stress evidence durability, reset ordering, shutdown and the provider prerequisite. Codex additionally found backup recovery can regress evidence, old backups can undo reset, aggregate caps are missing and crash tests need exact state oracles. These are accepted specification fixes under completeness, not claims of discovered code bugs.

1. Recovery uncertainty: a compatible backup may predate a hint or wrong answer. A recovered profile therefore has an explicit uncertain-history provenance flag. Previously completed immutable evidence in that backup is retained; unresolved assessments of the same catalogue revision cannot gain an independent-first label from that recovered history. They can be completed as practice with History recovered; first attempt unverified. This extra descriptive provenance text is a final-gate taste recommendation; do not falsely label unknown history either independent or definitely assisted. Temporary-session assistance is reconciled as assisted before assessment can resume; no new independence claim comes from unsaved/uncertain events.
2. Reset boundary includes every recovery source. Use an authoritative current-generation manifest with generation-specific snapshot/backup paths. Prepare the empty new generation, validate it, then atomically replace the manifest as the reset linearization point; do not select an older generation as fallback after a committed reset. A missing/corrupt manifest when profile files already exist enters recovery/temporary mode, not first launch. Only same-generation backups can restore active state. Preserve prior generation files for recovery without surfacing them as current progress; Reset progress is logical reset, not a claim of secure erasure. Unknown commit outcome requires rereading the manifest before retry/acknowledgement. File-operation implementation and flush/replace semantics must be proven on the selected OS; ordinary process-crash safety is the target, arbitrary power-loss durability is not claimed.
3. Shutdown stops request admission, invalidates contexts, cancels/discards queued AI work, wakes blocked publication, waits for transport completion bounded across DNS/connect/TLS/upload/read/redirect handling, joins worker, drains the ordered save writer or reports unsaved failure, then destroys shared resources. Mailboxes use nonblocking bounded publication or cancellation-aware waits, not an unbounded producer block. Exit while reset/evidence commit is pending must resolve the durable outcome before acknowledging success. A deadline parameter alone is not proof; selected transport prototype is mandatory.
4. Aggregate bounds supplement record bounds: catalogue input <=16 MiB, <=64 blocks/lesson, <=64 total visual records, parser nesting <=16 if a parser is used, layout-cache budget <=16 MiB, AI queue 1 active plus at most 1 pending admitted action, result mailbox 2. Store aggregate first-attempt facts and current solved/review membership, not unbounded answer/event history. Retain replay identity for the current/last committed operation plus monotonic sequence; prune redundant operations after committed snapshots. Maximum supported profile must serialize below the 2 MiB save limit in a generated worst-case fixture before those limits are accepted.
5. Crash checkpoints: before temp creation; during write; after validated/closed temp; before/after atomic replacement; before/after manifest switch for reset; before/after UI acknowledgement. Before reset manifest commit the old generation remains authoritative; after it only the new generation is authoritative. At uncertain return from replace, read and validate authority before proceeding. Assertions include event identity, first-result fact, assistance provenance, active generation, backup compatibility and user-visible saved/recovered/temporary state. Injected failures and isolated process termination are separate tests; neither proves power-loss behaviour.

### Engineering final disposition
Two original state races and four additional contract weaknesses now have explicit planned handling and test oracles. Provider/transport, target compatibility, font licensing and VoiceOver feasibility remain known prerequisites rather than silent gaps. Architecture is reviewed with concerns, not cleared as working code; no executable tests exist. Required engineering outputs are saved before starting DX.
## Phase 3.5 Developer experience review

### Applicability and persona
DX scope is present because the plan includes build commands, internal interfaces, authored lesson contracts, integration and debugging for two student developers. This is a narrow contributor workflow, not a public SDK, SaaS API or CLI product. Select DX POLISH: make the promised contributor path complete without adding a package manager, hosted playground or framework. The academic learner's first-run experience is also measured separately; do not confuse time to compile the application with time to finish a learner's first exercise.

### Developer perspective
I open README.md and see the CWaypoint title, the description of an AI-assisted C and C++ learning app, and a Status section saying implementation and build configuration are not yet added. I cannot find a prerequisite list, a configure command, a test command or a runnable example. I would not know whether my teammate's chosen compiler, raylib version and CPU architecture match mine. The Word work plans explain what I own, but they do not yet give me a working shared record to compile against. I can read the consolidated plan and see why saved evidence and AI context matter, but I still need a small real fixture to confirm that my evaluator output is what the UI expects. I want to build the offline app before I configure credentials. When a lesson record is rejected, I need the stable lesson/exercise ID, a named error and a correction I can make; I do not need a long provider payload. Before changing the catalogue I want a headless validator and tests to tell me whether examples, hints and revisions remain consistent. None of this is a failed runtime test today: there is no application to run. It is the documented contributor journey the implementation must create.

### Journey map and first-run roleplay
| Stage | Developer does | Current friction | Planned correction |
|---|---|---|---|
| 1 Discover | Reads README Status | No runnable app | State project stage honestly |
| 2 Prerequisites | Checks macOS/compiler/raylib | No versions or CPU matrix | Document tested toolchain and approved dependency source |
| 3 Configure | Runs CMake configure | CMakeLists absent | One proposed configure path with failures explained |
| 4 Build | Builds offline app | No target exists | Named application and headless test targets |
| 5 First value | Opens first real lesson | No binary/assets | Demo record included; no AI credential needed |
| 6 Real contribution | Adds a lesson/exercise | Contract only prose | Copyable checked fixture plus validator |
| 7 Debug | Triggers an invalid record/save failure | No diagnostics yet | Category, stable ID, cause and corrective action |
| 8 Upgrade | Updates code/catalogue | No compatibility evidence | Reader/version rules and preserved-profile fixture |
| 9 Feedback | Records setup friction | No baseline | Teammate clean-checkout timed record and issue notes |
Roleplay grounded in actual files: read README, look for build/setup, find none and consult the plan. Outcome is blocked by absent implementation; no elapsed time was measured. Initial time to hello world is unavailable; future target is under five minutes on an already provisioned teammate machine, with dependency installation recorded separately.

### Benchmark context and magical moment
Comparable contributor patterns are raylib's small working-window example and CMake's configure/build/test tutorial, not Stripe account onboarding or a hosted browser playground. Their official documentation demonstrates a small executable example and CTest registration; no comparable measured installation time was found or claimed. CWaypoint's intended first value is a real offline C lesson producing explanatory feedback, while its contributor first value is building that lesson and running the headless evaluator fixture. Deliver this as a documented local example, not new hosted infrastructure.

### Pass 1 Getting started
Initial 2/10 from README evidence; specification target 8/10. Put prerequisites, one build path and expected visible result before optional AI instructions. Proposed sequence after implementation: `cmake -S . -B build`, `cmake --build build`, then the documented actual application path; tests use `ctest --test-dir build --output-on-failure`. The generated target path and dependency setup must be verified rather than invented in README now; no claim that these commands work today.
A contributor should see CWaypoint Home and an available offline lesson without credentials. Configure failures should identify missing compiler, unapproved raylib dependency or unsupported configuration and a concrete setup step. Installation time remains outside the warm-start five-minute target and must be measured separately on clean machines. No new production dependency is installed or approved by this plan.

### Pass 2 Interfaces and naming
Initial 5/10; target 8/10. Keep operation grammar consistent: loadCatalogue, evaluateAnswer, applyLearningEvent, loadProgress, commitProgress, requestHint and pollHintResults are proposed internal names, not a public API commitment. Each returns a typed success/error record and stable IDs rather than magic sentinel strings. A minimal checked lesson record should show all required fields, and a fake hint result should show exactly which context fields the controller validates.
Defaults favour offline learning and one local profile. Complex provider settings appear only after the local workflow works; learners do not have to understand API schemas or file paths. Do not build a CLI/SDK wrapper, plugin system or public endpoint to make internal naming feel like a product. Time spent maintaining such a wrapper would not serve this two-person application.

### Pass 3 Actionable errors and debugging
Initial 4/10; target 9/10. The current repository has no runtime messages; all examples below are proposed, not observed output. Provide learner text separately from developer diagnostics so useful context never exposes credentials or learner answers.
| Path | Proposed learner message | Proposed developer diagnostic |
|---|---|---|
| Catalogue invalid | This question is unavailable. Choose another lesson. | CatalogueInvalid; lesson/exercise ID; missing accepted choice ID; update authored record; docs/content-contracts |
| Save write denied | Progress was not saved. Check available storage/access and Retry, or continue temporarily. | SaveWriteFailed; operation category; generation/sequence; isolated reproduction steps; no answer payload |
| AI not configured | AI hints are not configured. Authored hints are available. | AiNotConfigured; selected adapter/config ID; how to provide user credential without printing it |
Add a compact troubleshooting document covering these three paths plus timeout, invalid generation and unsupported save version. Logs must be bounded and sanitized. A verbose mode should not become a route to dumping secrets or provider bodies.

### Pass 4 Documentation and learning
Initial 3/10; target 8/10. README remains the entry point; link to setup, content contracts, test plan and work split. A contributor can follow one tutorial to add a lesson, then use a separate reference for complete record fields and revision rules. Keep authored content and AI examples synthetic and compiler-checked; no raw key or private save belongs in fixtures.
The current Markdown task lists and design Word file predate C++17/macOS and later decisions. Treat the reviewed plan as the current planning source and align historical documents only after approval. The two latest Word work plans already reflect the later split; do not silently claim they incorporate new autoplan recommendations. Copy-paste examples must be validated with the actual implementation before documentation acceptance.

### Pass 5 Upgrades and migration
Initial 5/10; target 8/10. Separate application build version, save schema version, catalogue format and assessment revision. Each future breaking change documents which reader supports which format and provides a compatible copy/recovery test before replacing the demo build. Unsupported versions remain preserved with actionable messaging; migrations are never implicit destructive rewrites.
Do not add a codemod system or automatic updater. This is a local academic build whose update path can be explicit and documented. The reset-generation and recovery-provenance rules in engineering define the state contract; documentation must explain logical reset versus secure erasure and avoid promising power-loss recovery.

### Pass 6 Environment and tools
Initial 2/10; target 8/10. Both teammates need a documented macOS/compiler/CPU configuration, approved raylib source/version, CMake configuration and an isolated test data directory. Headless core tests do not need a GPU, GUI session, live provider or secrets. The actual build should expose enough compile information for each teammate's editor through existing compiler tooling rather than introducing an IDE-specific dependency.
There is no runtime hot reload, CI/CD, container or cross-platform support to assess as present. Do not add any merely to improve a generic DX score; the user explicitly excluded deployment ceremony. Verify an artifact from another working directory with bundled approved font/assets and record which devices ran it.

### Pass 7 Contribution and ecosystem
Initial 4/10; target 7/10. For this project, the relevant community is the two teammates and academic reviewer, not a public plugin ecosystem. Define who authors records, who verifies examples, who owns integrated state and how contract changes are reviewed; both sides update together. This avoids learner-visible breakage from a unilateral field or enum change.
Repository visibility is not a licence grant for copied fonts, lessons or libraries. Choose and verify appropriate licensing before distribution; no licence text or dependency is silently selected by autoplan. No new chat channel, external messages, issue submission or public support commitment is authorised. AI requests must be explicit to avoid surprise costs; no particular provider price is assumed.

### Pass 8 Measurement and feedback
Initial 2/10; target 8/10. Record the actual clean-checkout steps and elapsed time for each teammate, plus separately the warm build and first offline exercise. The five-minute target is a goal for provisioned machines, not measured evidence. Compare failures and corrections to the documented golden path after each significant setup or contract change.
Use local notes on setup blockers, content-validator messages, test failures and learner confusion encountered during the team demonstration. Do not add hosted analytics, background telemetry or a research study. This enables a later DX audit to compare intended and actual steps without inventing current metrics or collecting private learner data.

### DX scorecard and checklist
| Dimension | Initial specification | Target specification | Acceptance evidence still needed |
|---|---|---|---|
| Getting started | 2 | 8 | Timed clean/provisioned teammate runs |
| Interfaces | 5 | 8 | Compiling fixture and consumer |
| Errors | 4 | 9 | Injected failures show cause/fix |
| Docs | 3 | 8 | Verified commands/examples |
| Upgrades | 5 | 8 | Compatibility/reset recovery fixtures |
| Environment | 2 | 8 | Named macOS devices and toolchain |
| Contribution | 4 | 7 | Shared contract review and licence check |
| Feedback | 2 | 8 | Actual local friction notes |
These are reviewer planning scores, not measured runtime DX. Required checklist: document approved prerequisites; implement offline configure/build/run/test path; supply lesson and fake-provider examples; add actionable troubleshooting; test profile compatibility; time both teammates' setup. No CLI skill appendix applies: this app is not an agent skill/MCP product.

### Decision audit additions DX
| ID | Phase | Decision | Classification | Principle | Rationale | Rejected |
|---|---|---|---|---|---|---|
| AP20 | DX | Review contributor DX, not a fictitious SDK | Mechanical | Pragmatic | Actual users are teammates and learners | Public API/platform scope |
| AP21 | DX | Offline first value without credential setup | Mechanical | Explicit | AI is optional in approved direction | Setup wizard before learning |
| AP22 | DX | Proposed CMake/CTest golden path with verified examples | Mechanical | Explicit | README currently has no build contract | Claiming commands already work |
| AP23 | DX | Local timed setup and error notes only | Mechanical | Pragmatic | Gives usable evidence without new infrastructure | Hosted analytics or CI/CD |

### DX implementation handoffs
Before splitting implementation, both teammates consume one shared synthetic contract fixture with all three exercise kinds, three authored hint levels, a separate fresh assessment, one invalid record and a versioned saved-evidence example. The first catalogue is typed bundled C++ records behind Catalogue; this avoids adding a parser dependency for the initial known content. Changing to an external content format is a separate architecture choice requiring both producer and consumer tests, not a hidden task for one teammate.
For local tests/demo, add a documented developer-only data-directory override (proposed CWAYPOINT_DATA_DIR) that selects an isolated fixture root and clearly labels the session. It must never be taken from lesson/model data, and tests must assert the normal profile was untouched. The exact default application-data location is a macOS implementation contract to verify, not a reason to write into the repository or home root arbitrarily.
Bootstrap failures before a window exists return nonzero and write a concise sanitized stderr diagnostic identifying category, failed asset/configuration and repair action. After a window exists, learner messages remain local and actionable; developer diagnostics include stable record ID and invalid field without raw answers or credentials. The offline first-value path must be documented and checked on the other teammate's Mac before M1 is accepted.
Add a contribution example for one exercise: choose stable ID and assessment revision, author record/answers/three hints, compile the example, run validator/evaluator checks, then integrate with the other owner's UI. Editorial changes retain assessment revision; changed question/answer semantics increment it. Code review is a handoff responsibility, not permission to commit or push during planning.

### Decision audit additions final handoffs
| ID | Phase | Decision | Classification | Principle | Rationale | Rejected |
|---|---|---|---|---|---|---|
| AP24 | Eng | Mark recovered unresolved evidence as unverified | Taste | Explicit | A backup can predate assistance or a wrong answer | Inventing independent/assisted certainty |
| AP25 | Eng | Reset authority spans generation manifest and backups | Mechanical | Completeness | Old backup must not undo acknowledged reset | Primary-only reset |
| AP26 | Eng | Add aggregate resource budgets and crash oracles | Mechanical | Completeness | Per-record caps miss combined exhaustion | Unbounded aggregate input |
| AP27 | DX | Share one compiling cross-owner fixture | Mechanical | DRY | Prevent divergent producer/consumer records | Parallel schema invention |
| AP28 | DX | Typed bundled records initially | Mechanical | Pragmatic | Known small authored catalogue needs no new parser | Unapproved parser dependency |
| AP29 | DX | Explicit isolated test/demo data root | Mechanical | Completeness | Tests must never change learner saves | Reusing normal profile for fixtures |
| AP30 | DX | Nonzero startup diagnostic before a window exists | Mechanical | Explicit | GUI cannot show an initialization failure yet | Silent exit |
| AP31 | DX | Keep provider/OS/CPU choices as recorded prerequisites | Mechanical | Explicit | No authority to invent compatibility or credentials | False readiness claims |

### DX outside review and synthesis
Codex supplied five findings; the independent agent supplied six; Claude remains unavailable. Required Claude/Codex six-dimension consensus is N/A, not confirmed. Both available reviewers found missing executable onboarding, cross-owner fixtures, actionable diagnostics and isolated-profile workflow. Those are now explicit milestone requirements rather than invented current capabilities.
Remove illustrative timing from evidence: observed sequence is simply read README, look for commands, find none, consult plan. No elapsed time was measured. Temporary-session troubleshooting must say Practice and authored hints are available; a new fresh check needs working progress storage. Save Retry retries a commit, AI Retry sends an explicit new provider request, and exercise Retry creates an explicit new attempt; labels and documentation must distinguish these actions.
Upgrade walkthrough copies the complete profile directory, including its authoritative manifest and generation files, into an isolated data root; never copy only progress.json and lose authority/provenance. Test the new build against that copy, preserve the original, and return to an older build only if its reader accepts the copied format. No migration or old-generation recovery is silently performed.
| Dimension | Claude | Codex | Independent agent | Consensus |
|---|---|---|---|---|
| Getting started | N/A | Blocked, no implementation | Not ready | N/A |
| Interfaces | N/A | Compiling fixture required | Conditional | N/A |
| Actionable errors | N/A | Tighten temporary guidance | Diagnostic detail needed | N/A |
| Docs | N/A | Entry point incomplete | Contribution flow incomplete | N/A |
| Upgrade | N/A | Profile-copy procedure needed | Isolation required | N/A |
| Environment | N/A | Device/toolchain unverified | Same | N/A |
DX phase complete as a planning review. Initial/target score mean is 3.4/8.0, not measured runtime quality. Time to first value is unknown; provisioned-machine target remains five minutes with dependency setup timed separately. All eight dimensions, developer narrative, nine-stage journey, roleplay, scorecard and checklist are written.

## Cross-phase themes
1. Evidence honesty recurred in CEO, design and engineering: independent first-attempt facts must survive transitions, remain descriptive and not imply retention or mastery.
2. A complete integrated slice recurred in every phase: the two owners must exchange compiling fixtures and demonstrate the learner journey before scaling authored content. This sequences rather than cuts the accepted scope.
3. Recovery and lifetime boundaries recurred in design, engineering and DX: source labels, temporary mode, manifest generations and worker shutdown must agree across UI, storage and diagnostics.
4. Unknown compatibility, live provider and custom-renderer accessibility remain repeated prerequisites. The reviewers agree these need evidence; neither availability of documents nor numerical review scores establishes that evidence.
These are repeated findings from Codex/independent host-model agents, not cross-model consensus with Claude.

## Final approval choices
- **C1 Curriculum budget:** recommend the proposed 14 existing topics, two practice questions plus one fresh check per lesson (42 questions and 126 authored hint levels), two walkthroughs and three comparisons. Alternative: retain feature scope but supply a different finite topic/question inventory. Deadline remains unknown.
- **C2 Failed hint requests:** recommend conservatively recording an admitted AI request as assistance even if it fails; it simplifies a durable, replay-safe boundary. Alternative: record assistance only once content is actually shown, requiring an additional durable delivery protocol. Neither permits help to be shown before its evidence is committed.
- **C3 Recovered uncertain history:** recommend History recovered; first attempt unverified for unresolved assessments after backup recovery. Alternative wording is welcome, but it must not assert knowledge the backup cannot establish.
- **C4 Visual/accessibility boundary:** recommend the light workspace and licensed fonts, keyboard/large-text acceptance, and an early VoiceOver feasibility prototype. Do not claim VoiceOver compatibility without the prototype. If full native screen-reader support is a course requirement and the prototype fails, return to the user before adding a native bridge or changing renderer; no such scope change is auto-approved.
No User Challenge changes the user's stated features. Provider, dependency approval, supported macOS/CPU baseline and academic deadline remain prerequisites rather than fabricated decisions. Claude and mockup tools were unavailable; the final plan is explicitly reviewed with concerns.

## State and failure diagrams
```text
Practice: Reading -> Awaiting answer -> Invalid (stay) / Incorrect -> Retry
                                      -> Correct -> Next / fresh check
Evidence: Not issued -> issued (commit) -> first submitted (commit)
                              |            +-> independent correct (no help)
                              |            +-> incorrect -> retry -> solved with practice
                              +-> assistance admission -> solved with practice
Recovery uncertain -> preserve known facts; unresolved first attempt unverified

Hint: Idle -> Loading -> Ready / Failed / Cancelled
       ^         |        |
       |         +-> stale context -> discard
       +--- cached reopen / explicit retry or Generate again

Save: Clean -> Dirty -> Saving -> Saved / Failed
                        |
Reset: stop old admission -> drain old replace -> prepare new generation
        -> commit authority manifest -> acknowledge -> accept new generation

Any expected failure -> typed result -> local user explanation + safe diagnostic
    content invalid -> isolate affected view/exercise
    storage uncertain -> preserve; recover or temporary
    AI unusable -> authored help; no grade mutation
    startup invalid -> sanitized stderr + nonzero exit
```
No existing code-comment diagram can be stale because no application source exists. The old design-document diagram omits approved additions and generation/evidence refinements; align it after approval rather than treating it as the implementation source.

## Review completion summary
| Field | Result |
|---|---|
| Mode and premises | SELECTIVE EXPANSION; human confirmed |
| Audit | Initial commit; README and planning docs; no application/build/tests |
| CEO | 11 sections evaluated; scope retained; finite inventory proposed |
| Design | 7 passes; written specification 5.0 -> 8.1; no generated mockups/visual verification |
| Engineering | 4 sections; evidence/reset/lifetime/recovery contracts specified; 22 test families planned |
| DX | 8 passes; written specification 3.4 -> target 8.0; actual setup duration unknown |
| Voices | Codex ran in all four phases; independent agents ran in all four; Claude unavailable |
| Consensus | Required Claude/Codex confirmed counts 0; remaining checks N/A, never fabricated |
| Critical gaps | No silently accepted plan failure; implementation and prerequisite checks all unrun |
| Diagrams | Architecture, shadow data paths, screen flow, state, error, distribution and rollback |
| Tasks | 16 unimplemented items, four JSONL phase artifacts and a human-readable aggregate |
| Follow ups | Six prerequisite/follow-up items recorded in TODOS.md |
| Scope changes | None to approved features; finite budget and evidence/provenance details await final gate |
| Deferred infrastructure | No accounts/backend/execution/hosted telemetry/CI/CD added |
| Reversibility | 4/5 planning judgment; save compatibility must be proven before data distribution |
| Status | APPROVED FOR PLANNING WITH CONCERNS; no runtime clearance |

## Artifact index
- docs/CWaypoint Implementation Tasks.md: 16 owner-labelled tasks with source finding, proposed paths, checks and rough effort.
- docs/CWaypoint Test Plan.md: 22 test families, complete learner journeys and exact crash/recovery/shutdown oracles.
- TODOS.md: prerequisite decisions, ownership, rationale and dependencies.
- .scratch/autoplan/: separate primary phase reports, outside reviewer reports, raw tool logs and aggregation input. These are local intermediate evidence, not release files.
- Local gstack restore point preserves the original decision history verbatim; source CEO history and existing Word documents remain unchanged.

## Approval record
On 6 November 2026 the user selected A at the autoplan final gate. C1 through C4 are accepted: 14 lessons / 42 questions / 126 authored hint levels; conservative assistance admission; recovered-history provenance; and the light workspace with early keyboard/large-text/VoiceOver feasibility checks. Original feature scope and ownership split remain intact. Approval authorises this planning record, not application implementation, dependency installation, commits or pushes.

## GSTACK REVIEW REPORT
| Review | Runs | Planning disposition | Remaining concerns |
|---|---|---|---|
| CEO | 1 | Approved with concerns | Course deadline/marking requirements still needed |
| Design | 1 | Approved with concerns | Font licensing, rendered checks and VoiceOver feasibility |
| Engineering | 1 | Approved with concerns | Provider/transport and macOS/CPU prerequisites; all runtime checks unrun |
| DX | 1 | Approved with concerns | Build/onboarding path and timings require implementation |
| Outside voices | 4 Codex + 4 independent agents | Completed with degraded model diversity | Claude unavailable; mockup generation unavailable |

**VERDICT:** The user approved the reviewed plan and all four final recommendations. Planning is complete with explicit implementation prerequisites. This is not runtime, build, visual, security-implementation or learning-efficacy clearance. No application code, commits or pushes were made.

**UNRESOLVED DECISIONS:**
- Course deadline and marking requirements before delivery scheduling.
- Actual supported macOS versions and CPU architectures before compatibility claims.
- Approved provider, credential source and networking dependencies before live AI integration.
- Font licensing and VoiceOver feasibility before related distribution/accessibility claims.
