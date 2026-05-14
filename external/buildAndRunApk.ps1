$currentLocation = $pwd

cp "D:\Github\Ajax Town Builds\android_arm64\libmain.so" "D:\Ajax Town Build\android-project\app\src\main\jniLibs\arm64-v8a" -Force

cd "D:\Ajax Town Build\android-project"

.\gradlew assembleDebug

adb install -r "D:\Ajax Town Build\android-project\app\build\outputs\apk\debug\app-debug.apk"

adb shell am start -n "com.gamedevajay.ajaxtown/org.libsdl.app.SDLActivity"

cd $currentLocation