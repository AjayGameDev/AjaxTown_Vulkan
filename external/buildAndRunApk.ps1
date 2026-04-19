$currentLocation = $pwd

cp "D:\Github\Ajax Town Builds\android_arm64\libmain.so" "C:\Users\dubey\CLionProjects\AjaxTown\build\deps_android_arm64\_deps\sdl-src\android-project\app\src\main\jniLibs\arm64-v8a" -Force

cd C:\Users\dubey\CLionProjects\AjaxTown\build\deps_android_arm64\_deps\sdl-src\android-project

.\gradlew assembleDebug

adb install -r C:\Users\dubey\CLionProjects\AjaxTown\build\deps_android_arm64\_deps\sdl-src\android-project\app\build\outputs\apk\debug\app-debug.apk

adb shell am start -n "com.gamedevajay.ajaxtown/org.libsdl.app.SDLActivity"

cd $currentLocation