#!/bin/sh
set -eu

cd "$(dirname "$0")"

python_bin=
for candidate in python3 python3.13 python3.12 python3.11 python3.14; do
    if command -v "$candidate" >/dev/null 2>&1 &&
        "$candidate" -c 'import sys; raise SystemExit(sys.version_info < (3, 11))' >/dev/null 2>&1; then
        python_bin=$candidate
        break
    fi
done

if [ -z "$python_bin" ]; then
    echo 'Python 3.11 or newer is required to start KisakCOD.' >&2
    exit 1
fi

"$python_bin" qualify_web_release.py verify .
exec "$python_bin" serve_web.py --directory site --port "${KISAK_PORT:-8000}"
