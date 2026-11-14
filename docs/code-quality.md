# Complexity and interface quality

CWaypoint uses **Lizard CCN ≤ 15 for every function under `src/`**. This includes
C/C++ headers and the Objective-C++ macOS bridge. Tests, examples and generated
build outputs are not application functions. No production file or function is
suppressed. The local check exits nonzero if a function exceeds 15; an empty scan
is an error rather than a passing result.

Cyclomatic complexity describes independent control-flow paths. The graph
formula is `M = E − N + 2P`; source analyzers approximate this from language
constructs. The threshold is our agreed project policy, not a certification or
proof of correctness. Read the [cyclomatic complexity reference](https://en.wikipedia.org/wiki/Cyclomatic_complexity)
and [Lizard documentation](https://github.com/terryyin/lizard).

## Run the checks

Development tools are optional and never bundled with the app:

```sh
python3 -m venv .scratch/quality-tools
.scratch/quality-tools/bin/python -m pip install -r requirements-quality.txt
make quality PYTHON=.scratch/quality-tools/bin/python
.scratch/quality-tools/bin/python scripts/check_complexity.py --json
```

`make quality` tests the gate with passing, failing and empty fixtures before
checking the application. Lizard 1.26.0 is pinned for reproducibility. Existing
CMake/CTest and `make test` remain the functional checks. There is no new CI or
hosted service.

Run [scc](https://github.com/boyter/scc) separately for file/language inventory:

```sh
scc src --format json
```

The comparison used scc 4.1.0. Its complexity estimate counts language-specific
branch tokens at file level; it is not interchangeable with Lizard's per-function
CCN. It is a secondary maintenance signal, not the threshold gate.

## Measured result — 13 November 2026

Baseline is commit `d496bcc`. Both versions were measured with the same tools and
source scope. Reproduce the baseline in an isolated directory with `git archive`.
The machine-readable summary is [complexity-metrics.json](complexity-metrics.json).

| Measurement | Before | After |
| --- | ---: | ---: |
| Application functions | 113 | 230 |
| Maximum function CCN | 288 | 15 |
| Functions above 15 | 17 | 0 |
| `main` CCN | 288 | 15 |
| Shared button CCN | 78 | 15 |
| Snapshot reader CCN | 57 | 15 |
| Answer-field renderer CCN | 69 | 6 |
| Lesson-screen dispatcher CCN | 54 | 4 |

The refactor separates command parsing, snapshot record validation, editing,
shared control drawing, navigation, learning stages, assistant conversation and
settings groups. Functions own cohesive work rather than suppressing warnings
or weakening validation. More small functions means more source lines; reduced
line count is not the goal.

Verification: seven CTest suites, the standalone lesson/C example checks, four
quality-gate contract tests and fourteen automated UI journeys passed. The
journeys include hints, review, undo, authored content, conditions, wide pointer
hits, all font sizes and four restart/resume phases. They exercise real handlers
and input models, not every physical macOS input path. They do not establish
complete path coverage, native accessibility or second-device compatibility.

## Design-tool findings applied

UI/UX Pro Max's local searches for `learning dashboard course progress` and
`focus not obscured` identified progress feedback and unobscured focus as useful
principles. The latter results describe web accessibility criteria; we apply the
layout principle without claiming native WCAG compliance. Impeccable's Operate
and craft guidance informed restrained controls and bounded visual inspection.
Its context launcher could not execute on this machine; PRODUCT.md and DESIGN.md
were read directly as prescribed by the skill.

Course progress now labels solved questions as well as completed modules, so the
bar's meaning is explicit. Main-page text, code, input rendering and pointer hit
boundaries share the same inset viewport, preventing content from drawing into
rounded panel margins. Course side panels keep independent clipping so their
anchored controls remain visible. Native captures were inspected at standard
and compact window sizes. CSV export remains absent from the application UI.

A later wide-window confirmation detected macOS restoring a narrower frame. The
pointer test now waits for and requests the required geometry with a bounded
timeout; the width assertion remains. The targeted pointer and course journeys
were rerun after this harness correction. Normal window behaviour is unchanged.


## Reference application — 14 November 2026

The native reference renderer, sandbox helper, multiline editor, personal storage and lab execution modules now measure 408 application functions with maximum Lizard CCN 15 and zero violations. `make quality PYTHON=.scratch/complexity-tools/bin/python` passes, including four gate tests.

scc 4.1 reports 30 source/header files, 4674 total lines, 4633 code lines and aggregate file-level complexity 1743. That aggregate is a different measure from the per-function Lizard limit.

Validation: 14 CTest suites pass; production wide and compact reference journeys verify saved completion, notes, bookmark, preferences, search and stale-result invalidation. Ten retained legacy feature journeys and four resume phases pass. The walkthrough-resume fixture initially omitted its saved-lesson prerequisite; reseeding and both walkthrough phases passed. The outdated lab-revision receipt test failed before the controller fix and passes afterward. Native keyboard/recording behavior and VoiceOver are not certified by these synthetic journeys.

The expanded stale-result journey exposed a responsive test-driver issue: after a native window enters compact mode, every code action must reopen the Code tab. The driver now uses the actual layout for all code actions, isolates synthetic input from live input, and captures failure state. Both layouts passed after that harness correction.

The final wide reference journey also enforces a width of at least 1280 px through bounded geometry settling, preventing macOS frame restoration from silently turning the wide check into a compact check.
