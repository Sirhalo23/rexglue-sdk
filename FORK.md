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
| When no audio device can be opened (none present, or the sound server not running), the SDL backend hands the game a silent driver that takes each frame and gives the client semaphore back at the pace of a real 48 kHz device, instead of failing `XAudioRegisterRenderDriverClient`. That call never fails on a console; Ridge Racer 6 crashed on a null object within seconds. The log says `No audio device could be opened; running without sound`. | Audio (`silent_audio_driver.cpp`, `sdl_audio_system.cpp`) | not yet |

The two changes from PR #487 were checked on Linux (Vulkan, llvmpipe): the
game's loading screen draws normally with the rebuilt runtime and no
validation errors are reported. They have not been tried on Windows or on a
Mac by this project.
