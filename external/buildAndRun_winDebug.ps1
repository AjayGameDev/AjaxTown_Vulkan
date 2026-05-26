$currentLocation = $PWD

$assetSourceLocation = "C:/Users/dubey/CLionProjects/AjaxTown/assets"
$assetTargetLocation = "D:/Github/Ajax Town Builds/VS Debug/Debug/assets"

# compile shaders
cd "$HOME/CLionProjects/AjaxTown/external"
./compileShaders.ps1

# then copy assets folder right next to exe
robocopy $assetSourceLocation $assetTargetLocation /E /XO /XD "astc" "android" "source" # /XF *.vert *.frag *.comp *.glsl

cd $assetTargetLocation
cd..


./main.exe

cd $currentLocation






