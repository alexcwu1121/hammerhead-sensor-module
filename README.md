# hammerhead-sensor-module

Electrical and software design files for the Hammerhead Sensor Module, top level control system of the Hammerhead mobile robot platform.

## 1. Software Development Prerequisites

Development for this project requires working within a devcontainer which has the Hammerhead Sensor Module Yocto SDK installed.

To build the Hammerhead Sensor Module Yocto image and SDK, you should have at least 50Gb of disk space.

1. Note the version of the SDK required by the devcontainer config at `.devcontainer/devcontainer.json`. This is the version of the Hammerhead Yocto manifest which produces the SDK intended to be used with this revision of the Hammerhead Sensor Module software.
2. Pull down the specified tag of the Hammerhead Yocto manifest repo and follow BitBake environment setup instructions: https://github.com/alexcwu1121/yocto-hammerhead
3. Build the core Hammerhead image with SDK. This can take a few hours depending on your build hardware: `bitbake -c populate_sdk hammerhead-core-image`
4. Copy the SDK installer script at `~/build/tmp/deploy/sdk/hammerhead-glibc-x86_64-hammerhead-core-image-cortexa76-hammerhead-toolchain-<version>.sh` to the root of this repo
5. Install Podman >4.x
6. Install VSCode
7. Install VSCode Dev Containers extension
8. In Dev Containers settings, set the following:
    - Docker Path: `podman`
    - Docker Compose Path: `podman-compose`
    - Docker Socket: `/run/podman/podman.sock`
9. Open VSCode to this directory and select from command pallette `Dev Containers: Rebuild and Reopen in Container`
10. VSCode will build the devcontainer, including copying in and installing the SDK. This may take several minutes.
