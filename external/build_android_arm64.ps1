$root = "C:\Users\dubey\CLionProjects\AjaxTown"

$ndk = "C:\Users\dubey\AppData\Local\Android\Sdk\ndk\29.0.14206865"

cmake -S "$root\external" -B "$root\build\deps_android_arm64" `
-G Ninja `
-DCMAKE_TOOLCHAIN_FILE="$ndk\build\cmake\android.toolchain.cmake" `
-DANDROID_ABI=arm64-v8a `
-DANDROID_PLATFORM=android-33 `
-DCMAKE_INSTALL_PREFIX="$root\build\deps_android_arm64\install" `


cmake --build "$root\build\deps_android_arm64" --target install