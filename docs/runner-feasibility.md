> **Current status — 14 November 2026:** The user approved WASI SDK 34 and Wasmtime 50.0.1. The pinned archives have been checksum-verified, their licences are bundled, and the offline sandbox helper and real C/C++ lab tests pass. Dependency-choice language below records the earlier feasibility stage. See [the implemented runtime contract](reference-runtime.md) for current setup, limits and validation evidence.

# Runner feasibility — 14 November 2026

The reference-design plan is approved. Its first implementation gate is real
isolated C/C++ compilation/execution; it has not yet passed.

## Signed helper experiment

A local Objective-C++ helper was built and ad hoc signed with App Sandbox enabled.
Its filesystem exceptions allowed reads of the installed toolchain/SDK and writes
only to the synthetic job workspace. Artifacts remain under `.scratch/helper-spike`.

| Experiment | Observed result |
| --- | --- |
| Build helper with explicit SDK | Passed |
| Sandbox helper compiles complete C++ hello-world | Passed; compiler exit 0 |
| Execute generated program | Denied: Operation not permitted |
| Repeat without the proposed process limit | Same denial; process limit was not the cause |
| Sign generated child with sandbox/inherit entitlements | Same denial |
| Add documented executable-file entitlement | Same denial |
| Inspect generated binary metadata | macOS quarantine attribute `0086` present |

The executable launch remains unverified. Quarantine is observed, but this report
does not claim the entire platform denial is fully diagnosed. No quarantine flag
was removed, system protection disabled, or learner program run unrestricted.
No production application source was changed by the experiment.

Apple documents sandbox inheritance and executable-file entitlements in
[Enabling App Sandbox](https://developer.apple.com/library/archive/documentation/Miscellaneous/Reference/EntitlementKeyReference/Chapters/EnablingAppSandbox.html).
The tested configurations did not produce a functioning runnable version on this
Mac. Stop iterating on that approach and choose a concrete execution alternative.

## Proposed replacement, dependency approval pending

Recommended: WASI SDK 34 (C/C++ compiler and standard libraries) with Wasmtime
50.0.1. CWaypoint remains a native macOS/raylib application. Exercise programs
compile to WebAssembly, with standard input/output, while the runtime exposes no
user filesystem or network capabilities. Runtime fuel/deadline/memory/output
limits and compiler-side file isolation still require actual integration tests.
Neither tool is downloaded, installed or bundled until approval.

Official Apple Silicon release archives inspected:

- `wasi-sdk-34.0-arm64-macos.tar.gz`: 180,322,002 bytes.
- `wasmtime-v50.0.1-aarch64-macos.tar.xz`: 9,463,036 bytes.

These are compressed download sizes, not installed or final bundle sizes.
Checksums, licences, packaging and C/C++ compatibility must be verified before use.

Sources: [WASI SDK](https://github.com/WebAssembly/wasi-sdk),
[Wasmtime security model](https://docs.wasmtime.dev/security.html),
[Wasmtime options](https://docs.wasmtime.dev/cli-options.html).

Alternative: Docker-backed compilation/execution with network disabled, resource
limits and scoped temporary files. The Docker CLI is installed, but `docker info`
currently cannot connect to its daemon. This option needs a running Docker
runtime and an approved compiler image; it is not bundled/offline-ready today.

The dependency choice is pending user response. The approved real-execution
feature is not silently replaced by authored answer checking or fake output.
