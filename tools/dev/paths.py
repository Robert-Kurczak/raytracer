"Project related paths in repository"

from pathlib import Path

# === Base ===
APPLICATION_NAME = "computer-graphics"

DEV_DIRECTORY_PATH = Path(__file__).parent
REPOSITORY_ROOT_PATH = DEV_DIRECTORY_PATH.parent.parent
BUILD_DIRECTORY_PATH = REPOSITORY_ROOT_PATH.joinpath("build")
SOURCE_DIRECTORY_PATH = REPOSITORY_ROOT_PATH.joinpath("src")
# ===

# === Files ===
APPLICATION_BINARY_PATH = REPOSITORY_ROOT_PATH.joinpath(
    f"build/{APPLICATION_NAME}"
)
# ===

# === Dockerfiles ===
BUILDER_IMAGE_NAME = "computer-graphics-dev"
BUILDER_DOCKERFILE_PATH = DEV_DIRECTORY_PATH.joinpath("Dockerfile")
# ===

# === Docker volumes ===
REPOSITORY_ROOT_MOUNT_PATH = REPOSITORY_ROOT_PATH
BUILD_DIRECTORY_MOUNT_PATH = BUILD_DIRECTORY_PATH
SOURCE_DIRECTORY_MOUNT_PATH = SOURCE_DIRECTORY_PATH
# ===
