#!/usr/bin/env python3

"""
Helper script for launching built application
"""

import sys
import subprocess
from dev import common
from dev import paths

def run_application():
    """
    Run built project
    """

    args = sys.argv[1:]

    try:
        subprocess.run(
            [paths.APPLICATION_BINARY_PATH, *args],
            cwd=paths.REPOSITORY_ROOT_PATH,
            check=True
        )
    except subprocess.CalledProcessError:
        common.log_error("=== Run failed ===")
        sys.exit(1)

    common.log_success("=== Run succeed ===")

if __name__ == "__main__":
    run_application()
