$currentLocation = $pwd

cd C:\Users\dubey\CLionProjects\AjaxTown\build\deps_android_arm64\_deps\sdl-src\android-project

.\gradlew assembleDebug

adb install -r C:\Users\dubey\CLionProjects\AjaxTown\build\deps_android_arm64\_deps\sdl-src\android-project\app\build\outputs\apk\debug\app-debug.apk

adb shell am start -n "com.gamedevajay.ajaxtown/org.libsdl.app.SDLActivity"

cd $currentLocation