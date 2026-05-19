$currentLocation = $PWD

$assetSourceLocation = "C:/Users/dubey/CLionProjects/AjaxTown/assets"
$assetTargetLocation = "D:/Github/Ajax Town Builds/VS Debug/assets"

robocopy $assetSourceLocation $assetTargetLocation /E /XO /XD astc /XF *.vert *.frag *.comp

cd $assetTargetLocation
cd..

./main.exe

cd $currentLocation