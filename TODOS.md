# CWaypoint Planning Prerequisites and Follow Ups
Planning output from autoplan on 6 November 2026. No implementation or external action is performed by this list.

## Required before implementation acceptance
- **P1 Shared build and device baseline.** Record actual teammate macOS versions/CPU architectures and an approved raylib/toolchain source. Why: no honest compatibility claim without device evidence. Effort: small setup experiment, estimates unvalidated. Depends on teammate device information. Owner: both, user leads build.
- **P1 Provider and networking contract.** Choose provider, credential source, endpoint/schema/refusal mapping, HTTPS/cancellation behaviour and any production dependencies before live integration. Why: a fake provider cannot prove actual lifecycle or privacy behaviour. Cost: prototype and synthetic live tests. Depends on explicit dependency/configuration approval. Owner: Oleksandr with both reviewing.
- **P1 Accessibility feasibility.** Prototype answer editing, focus, code scrolling and VoiceOver exposure in the raylib/macOS window. Why: custom drawing does not establish native accessibility. Cost: early device prototype; a bridge may require revisiting scope. Do not claim compliance without evidence. Owner: user, both verify.
- **P1 Curriculum acceptance and deadline.** Review the proposed 14-topic, 42-question, 126-authored-hint budget and actual marking requirements/date. Why: approved features must have a finite authoring/verification boundary. Depends on final plan approval and course brief. Owner: both, Oleksandr leads content.
- **P2 Font and asset licensing.** Select permitted redistributable fonts/assets and record sources/licences before bundling. Why: public repository visibility does not grant rights to third-party assets. Owner: user.
- **P2 Documentation alignment.** After plan approval, align original design and earlier Markdown task lists with the reviewed plan; regenerate Word work plans if requested. Why: earlier documents contain superseded choices and do not include every autoplan amendment. Do not silently replace delivered Word artifacts during this review. Owner: both.

## Explicitly outside approved first version
Arbitrary learner-code execution/sandboxing, accounts, cloud sync, leaderboards, mobile/Windows/Linux support, hosted backend/telemetry, CI/CD and public release/signing infrastructure. These are exclusions, not promises to build them later. Reconsider only on a new user instruction.

## Reference-design review — pending final approval (13 November 2026)

The new request explicitly reopens real learner-code execution, superseding that item in the earlier first-version exclusions once this plan is approved. It does not reopen hosted services, accounts, extra-language curricula or release automation. See `docs/reference-design-build-plan.md` for the implementation and test contracts.

- **P1 Runner feasibility, first implementation gate.** Prove a signed isolated helper can compile/run C/C++ and enforce resource/control-channel/child cleanup boundaries. Existing restrictive sandbox-exec compiler probes aborted before diagnostics; no unsafe fallback is authorized. Owner: application work; blocked on implementation proof, not a fake Run UI.
- **P2 Shared Discussion service deferred.** The requested tab is designed as explicitly offline with a Notes shortcut. Real shared discussions require an approved service, identity/moderation/privacy and dependency contract; no fake people/posts. Revisit if the user chooses that scope at final approval. Human effort: large; assisted estimate unvalidated; blocked on explicit service decision.
- Existing device baseline, native accessibility/IME, curriculum acceptance and font licensing prerequisites above remain relevant; reuse those follow-ups rather than duplicate them.
