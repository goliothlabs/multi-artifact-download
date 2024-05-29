# Multi-Artifact Download

## Overview

This application demonstrates how to use Golioth's Firmware SDK to download
multiple artifacts from a single release made in Golioth's OTA service. This
application waits to receive a release manifest from Golioth and then
downloads each component in the manifest and calculates it's SHA256 hash. You
can compare the hash calculated by the device with the hash displayed in the
Golioth Console to verify that the downloads completed successfully.

This functionality for downloading artifacts can be combined with additional
user-supplied code for storing and applying updates to implement complex
update scenarios involved (for example) multiple MCUs. Users will also need to
implement status notifications back to the Golioth Console. The simple, single
artifact fw_update module (at src/fw_update.c) can be used as an example for
how to report status updates.

## Setup

After cloning this repository, first update submodules:

```sh
git submodule update --init --recursive
```

Then install the system dependencies:

```sh
sudo apt install libssl-dev
```


## Building

First configure the build system with CMake

```sh
cmake -B build -S app
```

Then you can compile the aplication with make

```sh
make -j -C build
```

## Running

The application retrieves Golioth PSK credentials from the local environment.
These can be supplied at the command line when running the application:

```sh
GOLIOTH_PSK_ID="your-psk-id" GOLIOTH_PSK="your-psk" ./build/multi_artifact_download
```

To use the app, you need to create a release with multiple artifacts in the
Golioth Console. Once the release is rolled out, the application will
automatically start downloading the artifacts.
