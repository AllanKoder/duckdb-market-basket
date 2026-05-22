PROJ_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# Configuration of extension
EXT_NAME=market_basket
EXT_CONFIG=${PROJ_DIR}extension_config.cmake

# Include the Makefile from extension-ci-tools
include extension-ci-tools/makefiles/duckdb_extension.Makefile

# Regenerate compile_commands.json for LSP after builds
.PHONY: regenerate-compile-commands
regenerate-compile-commands:
	@if [ -d build ]; then \
		echo "Regenerating compile_commands.json for clangd..."; \
		cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON > /dev/null 2>&1 || true; \
	fi

# Hook into main build targets
debug: regenerate-compile-commands
release: regenerate-compile-commands
relassert: regenerate-compile-commands
reldebug: regenerate-compile-commands