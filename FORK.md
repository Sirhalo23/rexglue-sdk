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
