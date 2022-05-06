#!/bin/bash
set -xe

if [[ -v CLEANUP ]]; then
  echo "Cleaning up"
  rm -rf linux-build
fi

mkdir -p linux-build
cd linux-build

if [[ -z "${COVERAGE}" ]]; then
  cmake -GNinja -DCMAKE_BUILD_TYPE=Release ..
  cmake --build . --config Release
  ctest -VV -C Release
else
  find . -name "*.gcda" -exec rm \{} \;

  cmake -GNinja -DCMAKE_BUILD_TYPE=Debug -DCOVERAGE=ON ..
  cmake --build . --config Debug
  ctest -VV -C Debug
  files=`find . -name "*.gcda" | xargs`
  gcov -f -b $files
  lcov --directory . --base-directory . --capture -o cov.info
  lcov --remove cov.info '*/usr/include/*' '*test*' '*_deps*' -o coverage.info
  cat coverage.info | c++filt >coverage_filtered.info
  genhtml coverage_filtered.info --output-directory html
fi
