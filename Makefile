BUILD_DIR ?= build
CMAKE ?= cmake
BUILD_TYPE ?= Debug
CMAKE_ARGS ?=
TEST ?=

.PHONY: help configure build exec test tests perf benchmark all clean

help:
	@echo "Targets:"
	@echo "  make configure  Configure the CMake build tree"
	@echo "  make build      Build all default targets"
	@echo "  make exec       Build all executables under exec/"
	@echo "  make tests      Build and run all tests"
	@echo "  make tests TEST=<filter>  Build and run matching Catch2 tests"
	@echo "  make test       Alias for tests"
	@echo "  make perf       Build and run Google Benchmark perf targets"
	@echo "  make benchmark  Alias for perf"
	@echo "  make clean      Remove the build directory"

configure:
	$(CMAKE) -S . -B $(BUILD_DIR) -G Ninja -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) $(CMAKE_ARGS)

build: configure
	$(CMAKE) --build $(BUILD_DIR)

exec: configure
	$(CMAKE) --build $(BUILD_DIR) --target itch_exec

tests: configure
	$(CMAKE) --build $(BUILD_DIR) --target itch_tests
	@if [ -n "$(TEST)" ]; then \
		./$(BUILD_DIR)/tests/itch_tests "$(TEST)"; \
	else \
		ctest --test-dir $(BUILD_DIR) --output-on-failure; \
	fi

test: tests

perf: configure
	$(CMAKE) --build $(BUILD_DIR) --target itch_perf
	./$(BUILD_DIR)/perf/itch_perf

benchmark: perf

all: build tests perf

clean:
	$(CMAKE) -E rm -rf $(BUILD_DIR)
