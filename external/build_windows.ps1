$root = "C:\Users\dubey\CLionProjects\AjaxTown"

cmake -S "$root\external" -B "$root\build\deps_windows" `
-DCMAKE_BUILD_TYPE=Debug `
-DCMAKE_INSTALL_PREFIX="$root\build\deps_windows\install"

cmake --build "$root\build\deps_windows" --target install --config Debug