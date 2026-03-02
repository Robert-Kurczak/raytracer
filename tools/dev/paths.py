"Project related paths in repository"

from pathlib import Path

# === Base ===
APPLICATION_NAME = "computer-graphics"

DEV_DIRECTORY_PATH = Path(__file__).parent
REPOSITORY_ROOT_PATH = DEV_DIRECTORY_PATH.parent.parent
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
REPOSITORY_ROOT_MOUNT_PATH = DEV_DIRECTORY_PATH
BUILD_FOLDER_MOUNT_PATH = REPOSITORY_ROOT_MOUNT_PATH.joinpath("build")
SOURCE_FOLDER_MOUNT_PATH = REPOSITORY_ROOT_MOUNT_PATH.joinpath("src")
# ===
