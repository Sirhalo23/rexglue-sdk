# About this fork

This is a fork of the [ReXGlue SDK](https://github.com/rexglue/rexglue-sdk),
kept for the
[Ridge Racer 6 recompilation](https://github.com/Sirhalo23/RidgeRacer6-Recompilation).
It carries fixes that project needs before they are available in an upstream
release. It is not the official SDK; for anything else, use upstream.

- Branch `rr6` is upstream's `v0.10.0` plus the changes listed below.
- Branches `main`, `development` and `release/*` are upstream's, unchanged.
- Builds of this fork are tagged `v0.10.0.100`, `v0.10.0.101` and so on, so
  that they cannot be mistaken for an upstream version. The packages on the
  Releases page are built from those tags by the workflows in this repository.
- The licence is upstream's (see `LICENSE`), with its copyright notices kept.

## Changes relative to upstream v0.10.0

| Change | Area | Sent upstream |
| --- | --- | --- |
| The texture result exponent bias is read from fetch constant word 3, not word 4. With a LOD bias set, sampled colours were scaled by a power of two made out of that bias; Ridge Racer 6's track textures came out black. | Vulkan (`spirv_translator_fetch.cpp`) | not yet |
| macOS source builds: the version lookup, the MoltenVK ICD path and the ImGui include path work from an SDK checkout used by a game project; MoltenVK includes the upstream fix for 1x1 drawables after swapchain recreation; the guest output pass enables primitive restart, which Metal requires for strip topologies (no effect on the four non-indexed vertices it draws elsewhere). By Alan Bradburne, from upstream PR #487. | macOS, Vulkan presenter, build | PR #487 closed; upstream development addresses the core build/presentation issues |
| `window_high_pixel_density` (default true, as before) can turn off SDL's high-density back buffer, so a Retina display does not get a back buffer twice the size in each direction. By Alan Bradburne, from upstream PR #487. | UI (SDL window) | PR #487 closed; this optional control remains in the rr6 fork |
| MoltenVK 1.4.3 at `701747d6`, matching upstream development, and a macOS 14.0 deployment target in both Mac presets. | macOS dependencies/build | local review changes; not published |

The two changes from PR #487 were checked on Linux (Vulkan, llvmpipe): the
game's loading screen draws normally with the rebuilt runtime and no
validation errors are reported. The local ARM64 RR6 consumer build also loads
MoltenVK 1.4.3 on an Apple M4 Max, with every staged runtime binary recording
macOS 14.0 as its minimum OS. The tester confirmed it runs. This is not runtime
validation on macOS 14 hardware; Windows/Intel testing remains outstanding.
