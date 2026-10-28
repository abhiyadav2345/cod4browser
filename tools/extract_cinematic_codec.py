"""Extract the pinned FFmpeg source needed by the browser codec build."""

import argparse
import shutil
import tarfile
from pathlib import Path, PurePosixPath


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("archive", type=Path)
    parser.add_argument("destination", type=Path)
    parser.add_argument("version")
    args = parser.parse_args()

    prefix = f"ffmpeg-{args.version}"
    extracted = 0
    with tarfile.open(args.archive, "r:xz") as source:
        for member in source:
            parts = PurePosixPath(member.name).parts
            if not parts or parts[0] != prefix or ".." in parts:
                raise ValueError(f"Unexpected FFmpeg archive path: {member.name}")
            target = args.destination.joinpath(*parts)
            if member.isdir():
                target.mkdir(parents=True, exist_ok=True)
            elif member.isfile():
                target.parent.mkdir(parents=True, exist_ok=True)
                with source.extractfile(member) as input_file, target.open("wb") as output_file:
                    shutil.copyfileobj(input_file, output_file)
                extracted += 1
                if extracted % 500 == 0:
                    print(f"Extracted {extracted} FFmpeg source files", flush=True)
            else:
                raise ValueError(f"Unexpected FFmpeg archive member: {member.name}")
    print(f"Extracted {extracted} FFmpeg source files in total", flush=True)


if __name__ == "__main__":
    main()
