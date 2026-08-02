#!/bin/sh
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
test_binary=$(mktemp "${TMPDIR:-/tmp}/smart-chair-native-tests.XXXXXX")
trap 'rm -f "$test_binary"' EXIT HUP INT TERM

"${CXX:-c++}" \
  -std=c++17 \
  -Wall \
  -Wextra \
  -Werror \
  -pedantic \
  -I"$repo_root/src" \
  "$repo_root/test/native/test_posture.cpp" \
  "$repo_root/src/core/posture/matrix/matrix_zones.cpp" \
  "$repo_root/src/core/posture/matrix/matrix_posture.cpp" \
  "$repo_root/src/core/posture/tof/tof_posture.cpp" \
  "$repo_root/src/core/posture/tof/tof_posture_baseline.cpp" \
  "$repo_root/src/core/posture/tof/tof_posture_backlevels.cpp" \
  "$repo_root/src/core/posture/tof/tof_posture_tags.cpp" \
  "$repo_root/src/core/posture/tof/back_meta.cpp" \
  "$repo_root/src/core/posture/posture_classifier.cpp" \
  -o "$test_binary"

"$test_binary"
