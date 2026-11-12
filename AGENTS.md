# Repository Notes

This is a C++ project for working with NASDAQ TotalView-ITCH data.

- Do not edit C++ source, header, perf, or test files unless Terry explicitly asks for a specific C++ edit.
- Moving C++ files is allowed when the task is explicitly about reorganizing the repository.
- Raw ITCH data is mounted read-only in the devcontainer at `/data/itch` (`$ITCH_DATA_DIR`). Never write into it; put derived output under the workspace (e.g. `build/` or a gitignored output dir).
- Each `sandbox/` subproject should include a quick `readme.md` with relatively strict goals for that exercise.
- `exec/` and `sandbox/` subdirectories are picked up automatically when they contain a `CMakeLists.txt`.
