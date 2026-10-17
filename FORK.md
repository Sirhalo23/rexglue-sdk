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
| macOS source builds: the version lookup, the MoltenVK ICD path and the ImGui include path work from an SDK checkout used by a game project; MoltenVK is pinned to its upstream fix for 1x1 drawables after swapchain recreation (the picture shrank to one stretched pixel); the guest output pass enables primitive restart, which Metal requires for strip topologies (no effect on the four non-indexed vertices it draws elsewhere). By Alan Bradburne, from upstream PR #487. | macOS, Vulkan presenter, build | upstream PR #487 (open) |
| `window_high_pixel_density` (default true, as before) can turn off SDL's high-density back buffer, so a Retina display does not get a back buffer twice the size in each direction. By Alan Bradburne, from upstream PR #487. | UI (SDL window) | upstream PR #487 (open) |
| `XMASetLoopData` reads its second argument as the 12-byte `XMA_LOOP_DATA` it is, not as a whole XMA context. The loop start, end and count came from the wrong bytes (the start and end from past the end of the structure), so music streamed in blocks and looped with this call stopped after its first pass (Ridge Racer 6's menu music). | XMA (`xboxkrnl_audio_xma.cpp`) | not yet |
| `vsync_to_display` (default false): frames are shown at the display's vertical blank (no tearing), and on Direct3D 12 the guest's vertical blank follows the display's when the display refreshes at a whole multiple of the guest's rate (60 or 120 Hz for a 60 Hz title), so no frame is repeated or dropped by the two clocks drifting apart. Other displays, and Vulkan, keep the timer; Vulkan then presents in FIFO mode. The log says which is used. | Presenters, guest vertical blank (`graphics_system.cpp`) | not yet |

The two changes from PR #487 were checked on Linux (Vulkan, llvmpipe): the
game's loading screen draws normally with the rebuilt runtime and no
validation errors are reported. They have not been tried on Windows or on a
Mac by this project.
