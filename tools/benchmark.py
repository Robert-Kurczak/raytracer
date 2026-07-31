#!/usr/bin/env python3

"""
Measures renderer performance
"""

import argparse
import json
import os
import subprocess
import sys
import tempfile
import time
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path

from dev import common, paths


@dataclass
class BenchmarkSceneData:
    "Benchmark scene data"

    resolution_x: int
    resolution_y: int
    samples_per_pixel: int
    max_recursion_depth: int
    json_name: str
    output_name: str
    renders_per_measurement: int


@dataclass
class BenchmarkResults:
    "Taken measurements during benchmarking"

    render_times: list[float]
    average_render_time: float


@dataclass
class Args:
    "Command line arguments"

    benchmark_name: str
    split_renders: bool


BENCHMARK_SCENES = [
    BenchmarkSceneData(
        resolution_x=500,
        resolution_y=500,
        samples_per_pixel=50,
        max_recursion_depth=10,
        json_name="cornell-box-sphere-scene.json",
        output_name="benchmark-cornell-sphere.exr",
        renders_per_measurement=10,
    ),
    BenchmarkSceneData(
        resolution_x=1280,
        resolution_y=720,
        samples_per_pixel=10,
        max_recursion_depth=5,
        json_name="sponza-scene.json",
        output_name="benchmark-sponza.exr",
        renders_per_measurement=10,
    ),
]


def create_benchmark_directory(benchmark_name: str):
    """Create directory for current benchmark run"""

    timestamp = datetime.now(timezone.utc).strftime(
        "%Y-%m-%d-%H-%M-%S"
    )
    directory_path = paths.RENDERS_DIRECTORY_PATH.joinpath(
        f"benchmark-{benchmark_name}-{timestamp}"
    )

    os.makedirs(directory_path)

    return directory_path


def load_scene(
    scene_data: BenchmarkSceneData, output_directory: Path
) -> dict:
    "Loads initial scene json config"

    json_path = paths.SCENES_DIRECTORY_PATH.joinpath(
        scene_data.json_name
    )

    output_path = output_directory.joinpath(scene_data.output_name)

    scene = {}
    with open(json_path, "r", encoding="utf-8") as scene_file:
        scene = json.load(scene_file)

        scene["outputPath"] = output_path.as_posix()
        scene["renderer"]["pathsPerPixel"] = (
            scene_data.samples_per_pixel
        )
        scene["renderer"]["scatterRecursionDepth"] = (
            scene_data.max_recursion_depth
        )
        scene["camera"]["screenWidth"] = scene_data.resolution_x
        scene["camera"]["screenHeight"] = scene_data.resolution_y

        return scene


def measure_render_time(scene_json_path: str):
    "Measure time of scene rendering"

    try:
        start_time = time.perf_counter()

        subprocess.run(
            [paths.APPLICATION_BINARY_PATH, scene_json_path],
            check=True,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
        )

        return time.perf_counter() - start_time

    except subprocess.CalledProcessError:
        common.log_error("=== Run failed ===")
        sys.exit(1)


def prefix_output_file_name(
    output_directory: Path, output_name: str, prefix: int
):
    """
    Prefixes rendered image name with the given number
    and moves it to a separate directory
    """

    prefixed_name = f"{prefix}-{output_name}"
    initial_path = output_directory.joinpath(output_name)
    subdirectory_path = output_directory.joinpath(initial_path.stem)
    result_path = subdirectory_path.joinpath(prefixed_name)

    os.makedirs(subdirectory_path, exist_ok=True)
    os.rename(initial_path, result_path)


def benchmark_scene(
    scene_data: BenchmarkSceneData,
    output_directory: Path,
    split_renders: bool,
):
    "Perform render time measurements on given scene"

    common.log_info(f"Benchmarking {scene_data.json_name}")

    scene = load_scene(scene_data, output_directory)

    render_times = []
    average_render_time = 0.0

    with tempfile.NamedTemporaryFile(mode="w+") as temp_file:
        json.dump(scene, temp_file)
        temp_file.flush()

        renders = scene_data.renders_per_measurement

        for i in range(1, renders + 1):
            status_string = f"Render {i}/{renders}"
            status_end = "\r"

            if i == renders:
                status_end = "\n"

            common.log_info(status_string, end=status_end)

            render_time = measure_render_time(temp_file.name)

            render_times.append(render_time)
            average_render_time += render_time

            if split_renders:
                prefix_output_file_name(
                    output_directory, scene_data.output_name, i
                )

    average_render_time /= renders

    return BenchmarkResults(
        render_times=render_times,
        average_render_time=average_render_time,
    )


def create_benchmark_results_string(
    benchmark_name: str,
    scene_data: BenchmarkSceneData,
    results: BenchmarkResults,
):
    "Creats benchmark results text for given scene"

    return f"""
{benchmark_name}
=== Benchmark results ===
\tscene: {scene_data.json_name}
\twidth: {scene_data.resolution_x}
\theight: {scene_data.resolution_y}
\tsamples per pixel: {scene_data.samples_per_pixel}
\trenders: {scene_data.renders_per_measurement}

\taverage render time: {results.average_render_time}s

"""


def export_benchmark_results(
    benchmark_name: str,
    scene_data: BenchmarkSceneData,
    results: BenchmarkResults,
    output_directory: Path,
):
    "Exports and logs benchmark results"

    results_string = create_benchmark_results_string(
        benchmark_name, scene_data, results
    )

    render_times_string = ", \n".join(
        f"\t {x}" for x in results.render_times
    )

    common.log_info(results_string)

    result_file_path = output_directory.joinpath("results.txt")

    with open(result_file_path, "a", encoding="utf-8") as file:
        file.write(results_string)
        file.write(f"Render times: [\n{render_times_string}\n]")


def start_benchmark(args: Args):
    "Main benchmark loop"

    output_directory = create_benchmark_directory(args.benchmark_name)

    for scene_data in BENCHMARK_SCENES:
        results = benchmark_scene(
            scene_data, output_directory, args.split_renders
        )
        export_benchmark_results(
            args.benchmark_name, scene_data, results, output_directory
        )

    common.log_success("=== Benchmark successfull ===")


def parse_args() -> Args:
    """Parser command line arguments"""

    parser = argparse.ArgumentParser(
        prog="Benchmark",
        description="Benchmarks raytracer performance",
    )

    parser.add_argument(
        "-n",
        "--name",
        type=str,
        required=False,
        default="",
        help="Optional name for benchmark run",
    )

    parser.add_argument(
        "-s",
        "--split",
        action="store_true",
        required=False,
        help="Make each rendered image during measurement a separate file",
    )

    parsed = parser.parse_args()

    return Args(benchmark_name=parsed.name, split_renders=parsed.split)


def main():
    "Main script function"
    args = parse_args()
    start_benchmark(args)


if __name__ == "__main__":
    main()
