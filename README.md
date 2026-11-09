![version](https://img.shields.io/badge/version-20%2B-E23089)
![platform](https://img.shields.io/static/v1?label=platform&message=mac-intel%20|%20mac-arm%20|%20win-64&color=blue)
[![license](https://img.shields.io/github/license/miyako/4d-plugin-crash)](LICENSE)
![downloads](https://img.shields.io/github/downloads/miyako/4d-plugin-crash/total)

# 4d-plugin-crash

Crash 4D on purpose. The plugin has one command, [CRASH 4D](#crash-4d), which makes the 4D application that runs it terminate immediately, the way a real crash would. Use it to test what happens around a crash: crash reports, automatic restart scripts, backup and journal recovery, monitoring and alerting. It has no parameters and returns nothing.

| Command | Returns | Purpose |
|---|---|---|
| [CRASH 4D](#crash-4d) | — | Terminate the 4D application with a crash |

**Platforms:** macOS (Apple silicon and Intel), Windows (64-bit)

---

## Requirements & platform notes

- **Never install this plugin in a production application.** Any code that calls [CRASH 4D](#crash-4d), whether by design, by mistake or from a method someone else can trigger, takes the whole application down. Unsaved data in every process is lost.
- **The crash is not a 4D error.** `ON ERR CALL` doesn't see it, nothing after the call runs, and 4D gets no chance to save or close anything.
- **It crashes the application that executes the code.** On 4D remote it crashes that client. In a method that runs on the server (for example one with the "Execute on server" property), it crashes 4D Server and disconnects every client.
- **Thread safety:** the manifest declares the command thread-safe, so 4D accepts it in preemptive processes. The command hands the crash over to 4D's main process. Whether 4D performs that hand-off from a preemptive process has not been verified; test it on your version before relying on it there.
- Requires 4D 20 or later (see the badge). The macOS build targets macOS 11.5 or later.
- **Old `1.0.0` release.** The original `1.0.0` release (tag without `v`) contains only an Apple silicon (`arm64`) macOS binary, so it does not load on Intel Macs. `v1.0.0` and later, built by the release workflow, are universal (`arm64` and `x86_64`).

---

## CRASH 4D

### Syntax

```4d
CRASH 4D
```

| Parameter | Type | Description |
|---|---|---|
| Result | — | None. The command never returns: 4D terminates during the call |

### Description

`CRASH 4D` asks 4D to run a plugin function in the main process (`PA_RunInMainProcess`). That function writes to a NULL pointer, which the operating system treats as a fatal fault, so the 4D application terminates during the call.

**On macOS**, the compiler turns the NULL write into a trap instruction (checked in the disassembly of the current build), so the crash report shows a breakpoint or illegal-instruction exception (`EXC_BREAKPOINT` on Apple silicon, `EXC_BAD_INSTRUCTION` on Intel) rather than `EXC_BAD_ACCESS`. Expect the crashed thread to be 4D's main thread, with the plugin's `CRASH_4D` function at the top of its stack.

**On Windows**, the NULL write is expected to raise an access violation (`0xC0000005`). This is inferred from the source; the Windows build was not tested.

### Example

From the plugin's own test method (`TEST.4dm`):

```4d
CRASH 4D
```

Ask for confirmation first, so the crash can't happen by a stray click:

```4d
CONFIRM("Crash 4D now? Unsaved data will be lost.")
If (OK=1)
	CRASH 4D
End if
```

---

## Error handling & troubleshooting

- **Nothing can catch it.** `ON ERR CALL` and other 4D error handlers don't run, because the crash is a fault in native code, not a 4D error.
- **Unsaved data is lost.** Records being edited, variables and anything not yet written to the data file are gone, in every process. Check how your backup and journal settings handle the restart; see the 4D documentation on data file recovery for your version.
- **Only use a test data file.** A crash in the middle of writes is exactly when a data file may need verifying or repairing, so run these tests on a copy.
- **4D doesn't load the plugin on an Intel Mac.** You have the arm64-only `1.0.0` release. Use `v1.0.0` or later.
- **No crash when called in a preemptive process?** The preemptive case is not verified (see [Requirements & platform notes](#requirements--platform-notes)). Call [CRASH 4D](#crash-4d) from a cooperative process instead.

The test database also contains a method (`メソッド2.4dm`) that brings 4D down without the plugin, by allocating memory until it runs out. It is not part of the plugin:

```4d
C_COLLECTION:C1488($col)
$col:=New collection:C1472()

While (True:C214)
	$col.push(New collection:C1472(1000000))  // Keep allocating memory
End while 
```

---

## Quick reference

```4d
CRASH 4D  // 4D terminates here; nothing after this line runs
```

---

## Releases

Download the plugin from the [Releases](https://github.com/miyako/4d-plugin-crash/releases) page. Releases from `v1.0.0` are tagged `vX.Y.Z` and built, signed and notarized by **Build**.

### Maintainer lifecycle

**Bump version** is manual and source-only. Select the branch explicitly in
Actions, then choose patch, minor or major. It checks out immutable `github.sha`,
requires a clean checkout and an unchanged remote branch, validates the source
metadata, and commits exactly one direct child. A mandatory executable pre-push
guard checks the server-advertised old SHA against the selected SHA and accepts
exactly one update to the selected branch. Late advances, rewinds, deletions and
same-result races fail closed. The non-force push disables followed tags.
No tag, release or automatic Build is triggered. A rejected push requires a new
dispatch against the current branch; do not force-push the prepared commit.

**Build** is a separate manual dispatch with **zero inputs**. Select the desired
branch/commit in Actions (or `gh workflow run release.yml --ref BRANCH`).
Every job checks out the same immutable `github.sha`. The strict source `X.Y.Z`
determines `vX.Y.Z`, `crash-X.Y.Z.zip` and `crash-X.Y.Z.dmg`; existing tags
cannot be used to rebuild or overwrite a release.

Source authority is the coordinated `MARKETING_VERSION` and
`CURRENT_PROJECT_VERSION` in all three project configurations, as promised by
the original Bump header and the modernization commit `0b4478e`. Both must
agree with source `Info.plist` and both localized display strings in
`English.lproj/InfoPlist.strings`. Applicable literal keys are co-bumped;
supported Xcode references stay references and absent `CFBundleVersion` stays
absent. Prebuilt `test/Plugins`, native dependencies, 4D project/data files,
schema and unrelated counters are not version authority or bump targets.
This migration keeps the existing `1.0.0`; no stale source repair was needed.

Build serializes publication repository-wide across branches without cancelling
running jobs. It checks remote tags and authenticated, paginated releases
(including drafts) before any native work and again immediately after every
build/package/sign/notarize/staple gate. All pages and entries must validate;
404, authentication, network and malformed responses are errors, not absence.
Only a complete successful no-match permits creation.

Publication creates a new lightweight tag at the exact selected SHA and verifies
it remotely, then POSTs a new non-draft, non-prerelease release with empty notes,
preserving the original notes policy. Assets are separately POSTed to the
returned new release ID. It never updates, deletes, clobbers, rolls back or
retries remote state, including when another publisher wins a same-SHA race.

### Native gates and recovery

Windows still uses Release/x64 MSBuild with v143 and Windows SDK 10.0.
macOS still uses Xcode's Default configuration, macOS 11.5, and both arm64 and
x86_64; the legacy Deployment-only local notarization phase remains unchanged.
Committed resources and Windows binaries are merged into the same bundle.
Universal-binary/resource checks, Developer ID signing verification, ZIP and
DMG notarization acceptance and DMG stapling remain mandatory. The existing
certificate/keychain/notary secret names are unchanged; unavailable credentials
or native tooling must fail rather than bypass these gates. The 4D test methods
intentionally crash the host and are not executed by these workflows.

If publication fails after creation, the tag, release and any earlier uploaded
assets remain. Inspect the workflow error and retained remote state manually.
Do not rerun the same version expecting automatic repair: availability checks
will reject it. A maintainer must decide how to recover the partial release;
for a new complete publication, bump source metadata and dispatch Build again.
This automation performs no cleanup or overwrite.

Local non-native regressions: `python3 -B -m unittest discover -s .github/tests`.
Workflow lint: `actionlint .github/workflows/*.yml`.
