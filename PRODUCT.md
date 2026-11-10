# CWaypoint

CWaypoint is an academic macOS desktop application for a beginner learning C and C++ in separate tracks. The application is implemented in C++17 with raylib. Its central learning flow is explanation → prediction/practice → feedback → fresh check → summary, with local progress and optional help.

The current executable is the application/UI owner's first working increment. It contains three starter lessons (C variables, C pointers, C++ strings), nine deterministic questions, three offline hint levels per question, two authored walkthroughs, three language comparisons, saved progress, review mistakes, generation-based reset and text-size settings. It makes no network requests. The full approved curriculum is 14 lessons, 42 questions and 126 hint levels; the remaining content and live AI module belong to Oleksandr's work.

The user owns application setup, navigation, controls, learning-state transitions, persistence, walkthrough player, comparison layout and integration. Oleksandr owns the complete authored catalogue, deterministic evaluator, teaching records and AI provider/worker. The included starter evaluator and content establish a compiling handoff; they do not represent completion of his full assignment.

An independent first attempt is historical evidence about that attempt, separate from practice completion or eventual success after help. Do not claim mastery or learning efficacy from a single result. Offline practice and authored hints must remain useful without credentials or internet access. Never execute learner input or grade using AI.

The implemented interface uses charcoal surfaces, periwinkle primary actions, top navigation, a logo Home link, a gear settings menu and fixed rounded learning panels. Home and Courses show both language courses together. Course screens contain a left module list, central material/practice and a right offline study assistant. Typed assistant messages return authored lesson notes or successive hints, not generated AI responses. Help still uses durable controller evidence; reset clears assistant conversations. Save problems link to full details. See [DESIGN.md](DESIGN.md) for tokens and layouts, [docs/content-contracts.md](docs/content-contracts.md) for compiling interfaces and [docs/implementation-status.md](docs/implementation-status.md) for actual validation and outstanding work.

Constraints: one local profile, macOS first, no hosted services/accounts/cloud sync, no new production dependencies without approval, no CI or release automation. This local Apple Silicon build is ad hoc signed, not notarized; other devices, minimum OS, native VoiceOver access and Unicode/IME text editing remain unverified or unimplemented. Custom single-line ASCII fields support cursor editing and selection; drafts survive navigation within the running session.

The guided path derives next-lesson suggestions from unfinished practice in the chosen track, including earlier gaps. Fresh checks do not block progression. Invalid optional teaching records show an unavailable state without blocking valid practice. These features operate on the three available starter lessons; they do not imply the full curriculum is delivered.

A dedicated mistake-review workflow filters the current queue by track and guides retries using the same evaluator/controller as ordinary practice. It preserves attempt/assistance facts and excludes obsolete assessment revisions. No review status claims long-term mastery.

Continue restores an exact question or walkthrough step after reopening. Local snapshot version two adds position/revision fields while reading version one and preserving backups. Draft answers remain session-only. Older builds preserve unsupported newer saves instead of overwriting them.

Code panels support full-source copying and independent horizontal reading at the learner's text size. Source is authored teaching content; copying never executes it. Offsets are session-local and separate from saved learning evidence.

Text-answer drafts have bounded undo/redo history within the running session. This reverses editing only, never grading or attempt history.
