# nasdaq_itch

C++23 tooling for NASDAQ TotalView-ITCH data.

Layout:

- `lib/` - static `nasdaq_itch::itch` library; headers and sources live under `lib/itch/`.
- `tests/` - Catch2 tests (`itch_tests`) linking the library. Any `*.cc` under `tests/` is picked up.
- `perf/` - Google Benchmark targets (`itch_perf`). Any `*.cc` under `perf/` is picked up.
- `exec/` - executables that use the library; each subdirectory with a `CMakeLists.txt` is added.
- `sandbox/` - small exploratory subprojects; same auto-discovery as `exec/`.
- `.devcontainer/` - Ubuntu dev container with Claude Code, works on macOS or Windows hosts.

## Data

The dev container mounts `<host home>/data/itch` read-only at `/data/itch`
(also exported as `$ITCH_DATA_DIR`). Set `HOST_ITCH_DATA_DIR` in
`.devcontainer/.env` to use a different host directory.

## Build

```sh
make build        # configure + build (Ninja, Debug)
make tests        # build and run tests
make perf         # build and run benchmarks
```

Use `-DITCH_BUILD_PERF=OFF` (or `make build CMAKE_ARGS=-DITCH_BUILD_PERF=OFF`) to skip Google Benchmark targets.
