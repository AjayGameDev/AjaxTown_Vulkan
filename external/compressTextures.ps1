# output folders
$astc_directory = "C:/Users/dubey/CLionProjects/AjaxTown/assets/textures/astc"
$bc_directory = "C:/Users/dubey/CLionProjects/AjaxTown/assets/textures/bc"

# source folder for all textures
$source_directory = "D:/Ajax Town"

$files = dir $source_directory -Recurse -Include *.png, *.exr

foreach($file in $files)
{
    if($file.BaseName -like "*_diffuse*")
    {


        # astc compression
        nvtt_export `
              --format astc-ldr-4x4 `
              --mips `
              --export-transfer-function srgb `
              --output "$astc_directory/$($file.BaseName).ktx2" `
              "$($file.FullName)"

        # equivalent bc7 conversion
        nvtt_export `
              --format bc7 `
              --mips `
              --export-transfer-function srgb `
              --output "$bc_directory/$($file.BaseName).ktx2" `
              "$($file.FullName)"
    }
    elseif($file.BaseName -like "*_normal*")
    {
        # astc compression with two channel normal map, astc writes one channel in rgb and other in a
        nvtt_export `
              --format astc-ldr-4x4 `
              --mips `
              --export-transfer-function linear `
              --normalize `
              --output "$astc_directory/$($file.BaseName).ktx2" `
              "$($file.FullName)"

        # equivalent bc5 two channel conversion
        nvtt_export `
              --format bc5 `
              --mips `
              --export-transfer-function linear `
              --normalize `
              --output "$bc_directory/$($file.BaseName).ktx2" `
              "$($file.FullName)"
    }
    elseif($file.BaseName -like "*_rmao*")
    {
        # astc compression
        nvtt_export `
              --format astc-ldr-6x6 `
              --mips `
              --export-transfer-function linear `
              --normalize `
              --output "$astc_directory/$($file.BaseName).ktx2" `
              "$($file.FullName)"

        # equivalent bc
        nvtt_export `
              --format bc7 `
              --mips `
              --export-transfer-function linear `
              --output "$bc_directory/$($file.BaseName).ktx2" `
              "$($file.FullName)"
    }
}