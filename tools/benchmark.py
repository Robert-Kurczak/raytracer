#!/usr/bin/env python3

"""
Renders scene in verbose mode
with easily modifiable parameters via command-line
"""

import json
import subprocess
import sys
import tempfile
import argparse
from dataclasses import dataclass
from dev import common, paths

SCENE_JSON_PATH = paths.SCENES_DIRECTORY_PATH.joinpath(
    "cornell-box-sphere-scene.json"
)

DEFAULT_OUTPUT_PATH = paths.RENDERS_DIRECTORY_PATH.joinpath(
    "benchmark-scene.ppm"
)

@dataclass
class Args:
    "Command line arguments"

    camera_position: list[float]
    camera_direction: list[float]
    up_direction: list[float]
    vertical_fov: float
    light_colors: list[list[int]]
    light_positions: list[list[float]]
    resolution: list[float]
    recursion_depth: int
    output_path: str
    samples_per_pixel: int

def load_scene():
    "Loads initial scene json config"

    with open(SCENE_JSON_PATH, "r", encoding="utf-8") as scene_file:
        return json.load(scene_file)

def modify_scene(scene: dict, args: Args):
    "Modifies scene by applying command line arguments"

    scene["outputPath"] = DEFAULT_OUTPUT_PATH.as_posix()
    scene["verbose"] = True

    if args.camera_position:
        scene["camera"]["position"] = args.camera_position

    if args.camera_direction:
        scene["camera"]["direction"] = args.camera_direction

    if args.up_direction:
        scene["camera"]["upDirection"] = args.up_direction

    if args.vertical_fov:
        scene["camera"]["fieldOfViewDegrees"] = args.vertical_fov

    if args.resolution:
        scene["camera"]["screenWidth"] = args.resolution[0]
        scene["camera"]["screenHeight"] = args.resolution[1]

    if args.recursion_depth:
        scene["renderer"]["scatterRecursionDepth"] = args.recursion_depth

    if args.samples_per_pixel:
        scene["renderer"]["samplesPerPixel"] = args.samples_per_pixel

    if args.output_path:
        scene["outputPath"] = args.output_path

    if args.light_positions:
        scene["lights"] = []

        for index, position in enumerate(args.light_positions):
            color = args.light_colors[-1]

            if len(args.light_colors) >= len(args.light_positions):
                color = args.light_colors[index]

            light_json = {
                "type": "point",
                "position": position,
                "color": color,
                "decay": 1
            }

            scene["lights"].append(light_json)

def render_scene(scene: dict):
    "Executes raytracer to render benchmarked scene"

    with tempfile.NamedTemporaryFile(mode="w+") as temp_file:
        json.dump(scene, temp_file)

        temp_file.flush()

        try:
            subprocess.run(
                [paths.APPLICATION_BINARY_PATH, temp_file.name],
                cwd=paths.REPOSITORY_ROOT_PATH,
                check=True
            )
        except subprocess.CalledProcessError:
            common.log_error("=== Run failed ===")
            sys.exit(1)

        common.log_success("=== Run succeed ===")

def parse_arguments() -> Args:
    "Creates command line argument parser"

    parser = argparse.ArgumentParser(
        prog="Raytracer benchmark",
        description="Renders sample scene in verbose mode"
                    "with easily modifiable parameters via command-line"
    )

    parser.add_argument(
        "-vp",
        nargs=3,
        type=float,
        help="Camera position [x y z]",
    )

    parser.add_argument(
        "-vd",
        nargs=3,
        type=float,
        help="Camera direction [x y z]",
    )

    parser.add_argument(
        "-up",
        nargs=3,
        type=float,
        help="Up direction [x y z]",
    )

    parser.add_argument(
        "-fovy",
        type=float,
        help="Vertical field of view [degrees]",
    )

    parser.add_argument(
        "-ltcol",
        nargs=3,
        type=int,
        action="append",
        help="Lights color [r g b]",
    )

    parser.add_argument(
        "-ltpos",
        nargs=3,
        type=float,
        action="append",
        help="Light position [x y z]",
    )

    parser.add_argument(
        "-r",
        type=int,
        help="Recursion depth"
    )

    parser.add_argument(
        "-res",
        nargs=2,
        type=int,
        help="Image resolution [x y]"
    )

    parser.add_argument(
        "-o",
        type=str,
        help="Output file path"
    )

    parser.add_argument(
        "-s",
        type=int,
        help="Samples per pixel"
    )

    parsed_args = parser.parse_args()

    return Args(
        camera_position = parsed_args.vp,
        camera_direction = parsed_args.vd,
        up_direction = parsed_args.up,
        vertical_fov = parsed_args.fovy,
        light_colors = parsed_args.ltcol,
        light_positions = parsed_args.ltpos,
        recursion_depth = parsed_args.r,
        resolution = parsed_args.res,
        output_path = parsed_args.o,
        samples_per_pixel=parsed_args.s
    )

def main():
    "Main script function"
    args = parse_arguments()

    scene = load_scene()
    modify_scene(scene, args)
    render_scene(scene)

if __name__ == "__main__":
    main()
