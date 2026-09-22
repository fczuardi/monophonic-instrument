workspace_core := justfile_directory() + "/../.platformio-home"
pio := "PLATFORMIO_CORE_DIR=" + workspace_core + " pio"
click_app := "apps/plus2-click-renderer-probe"
plus2_app := "apps/plus2-buzzer-local-test"
core_gray_app := "apps/core-gray-speaker-local-test"

default:
    @just --list

# Run the pure monophonic-instrument package tests.
test:
    {{pio}} test -d packages/monophonic-instrument -e native

# Build the current click-renderer hardware probe.
build: build-click

build-click:
    {{pio}} run -d {{click_app}}

upload-click:
    {{pio}} run -d {{click_app}} --target upload

monitor-click:
    {{pio}} device monitor -d {{click_app}} --baud 115200

upload-monitor-click: upload-click
    {{pio}} device monitor -d {{click_app}} --baud 115200

build-plus2:
    {{pio}} run -d {{plus2_app}}

upload-plus2:
    {{pio}} run -d {{plus2_app}} --target upload

monitor-plus2:
    {{pio}} device monitor -d {{plus2_app}} --baud 115200

build-core-gray:
    {{pio}} run -d {{core_gray_app}}

upload-core-gray:
    {{pio}} run -d {{core_gray_app}} --target upload

monitor-core-gray:
    {{pio}} device monitor -d {{core_gray_app}} --baud 115200

# Exercise the package and every checked-in hardware consumer.
check: test build-click build-plus2 build-core-gray

devices:
    {{pio}} device list

clean-click:
    {{pio}} run -d {{click_app}} --target clean
