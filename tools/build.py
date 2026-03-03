#!/usr/bin/env python3

"""
Helper script for building project in a dev-container
"""

import sys
import subprocess
import os
import argparse
from dev import common
from dev import paths

def is_docker_image_built():
    """
    Checks whether dev-container image is available on host machine
    """

    result = subprocess.run(
        ["docker", "images", "-q", paths.BUILDER_IMAGE_NAME],
        stdout=subprocess.PIPE,
        stderr=subprocess.DEVNULL,
        text=True,
        check=False
    )
    return result.stdout != ""

def build_docker_image():
    """
    Build dev-container image on host machine
    """

    common.log_info("=== Building docker image ===")

    try:
        subprocess.run(
            [
                "docker", "build",
                "--tag", paths.BUILDER_IMAGE_NAME,
                "--file", paths.BUILDER_DOCKERFILE_PATH,
                "."
            ],
            check=True
        )
    except subprocess.CalledProcessError:
        common.log_error("=== Build failed ===")
        sys.exit(1)

    common.log_success("=== Build succeed ===")

def build_application(clean):
    """
    Builds application in a dev-container
    """

    clean_command = f"rm -rf {paths.BUILD_DIRECTORY_MOUNT_PATH}"
    cmake_command = str(
        "cmake"
        " -DCMAKE_BUILD_TYPE=Release"
        f" -S {paths.SOURCE_DIRECTORY_MOUNT_PATH}"
        f" -B {paths.BUILD_DIRECTORY_MOUNT_PATH}"
    )
    build_command = f"cmake --build {paths.BUILD_DIRECTORY_MOUNT_PATH} --parallel 16"

    full_command = f"{cmake_command} && {build_command}"

    if clean:
        common.log_info("=== Cleaning build directory ===")
        full_command = f"{clean_command} && {full_command}"

    try:
        subprocess.run(
            [
                "docker", "run", "--rm", "-it",
                "-u", f"{os.getuid()}:{os.getgid()}",
                "--volume", f"{paths.REPOSITORY_ROOT_PATH}:{paths.REPOSITORY_ROOT_MOUNT_PATH}:z",
                paths.BUILDER_IMAGE_NAME,
                "sh", "-c",
                f"{clean_command} && {cmake_command} && {build_command}"
            ],
            check=True
        )
    except subprocess.CalledProcessError:
        common.log_error("=== Build failed ===")
        sys.exit(1)

    common.log_success("=== Build succeed ===")


def main(clean):
    """
    Main script function
    """

    if not is_docker_image_built():
        build_docker_image()

    build_application(clean)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog="Erosion Simulator Builder",
        description="Builds application inside the dev container"
    )

    parser.add_argument(
        "-c", "--clean",
        action="store_true",
        help="Clean target before build"
    )

    args = parser.parse_args()

    main(args.clean)
