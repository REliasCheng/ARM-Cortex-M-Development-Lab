# Local asset and source boundary

Before implementing the control core, the repository's pre-remediation Git
tree and the local copyright decision list were checked in a bounded scope.
That historical tree contained 19 `.uvprojx` projects, 5 `.ioc` files, and
85 startup assembly paths, plus course application and board-support source.
These counts identify assets for review; they do not prove the projects build
today or grant redistribution rights. The current default branch has none
of those target files.

| Class | Finding | Use in this stage |
| --- | --- | --- |
| `USER_ORIGINAL` | Current repository-authored docs, diagrams, and this independently written control core | Published under the repository's MIT license |
| `LICENSED_REUSABLE` | Some historical GD32/CMSIS components have identifiable BSD-style notices in the local license matrix | Not copied; exact version, notice chain, target compatibility, and build need a separate decision |
| `REFERENCE_ONLY` | Historical Keil/CubeMX project layouts and peripheral choices | Used only to identify useful software boundaries; no project file or configuration copied |
| `RIGHTS_UNVERIFIED` | Historical course application and board-support source, and unrelated images | Excluded from this public implementation |
| `BUILD_EVIDENCE_ONLY` | Historical project/build records noted in the engineering handoff | Not treated as a build of the current source |

No original C/H application module in the conservative decision list was
marked `KEEP_CLEAR` for direct reuse. Accordingly, this stage implements a
small portable application core without vendor headers or course code.
There is no public target adapter, startup file, linker/scatter file, or
GD32/STM32 target binary here. On the current Windows host, `gcc` is
available, while `clang`, `arm-none-eabi-gcc`, and `UV4` were not found on
`PATH` or the checked standard Keil locations. This is not an exhaustive
license or machine inventory.

The next target-build stage requires a selected chip/board, proven pin and
clock configuration, a legally usable device support package, a target
toolchain, and a reviewed linker/interrupt contract. Host tests must not be
reported as board-level evidence.
