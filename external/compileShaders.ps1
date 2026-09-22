$currentLocation            =  "$PWD"
$shaderSourceLocation       =  "$HOME/CLionProjects/AjaxTown/assets/shaders/source"
$shaderDestinationLocation  =  "$HOME/CLionProjects/AjaxTown/assets/shaders/compiled"

cd $shaderSourceLocation

# All executable files use .slang, header assets track .slangh
$shaders = dir $shaderSourceLocation -Recurse -Include *.slang
$headers = dir $shaderSourceLocation -Recurse -Include *.slangh

$platforms = @(
    @{ name = "android"; define = "TARGET_PLATFORM_ANDROID"; textureFormat = "astc" },
    @{ name = "win32";   define = "TARGET_PLATFORM_WIN32";   textureFormat = "bc"   }
)

foreach($platform in $platforms)
{
    $outputLocation = "$shaderDestinationLocation/$($platform.name)"

    if(! (Test-Path ($outputLocation) ))
    {
        mkDir $outputLocation | Out-Null
    }

    foreach($shader in $shaders)
    {
        # Inspect code to find if it is a compute module
        $isCompute = Select-String -Path $shader.FullName -Pattern '\[shader\("compute"\)\]' -Quiet

        $checkExtension = if ($isCompute) { "comp.spv" } else { "vert.spv" }
        $spv = dir "$outputLocation/$($shader.BaseName).$checkExtension" 2>$null

        if($spv -eq $null -or $shader.LastWriteTime -gt $spv.LastWriteTime -or ($headers | Where-Object{$_.LastWriteTime -gt $spv.LastWriteTime}))
        {
            echo "Compiling [$($platform.name)]: $($shader.Name)"

            # CASE A: Compute Shader Pass
            if ($isCompute)
            {
                $compArgs = @(
                    $shader.FullName,
                    "-entry", "main",
                    "-stage", "compute",
                    "-target", "spirv",
                    "-D$($platform.define)",
                    "-o", "$outputLocation/$($shader.BaseName).comp.spv"
                )
                & slangc @compArgs 2>&1 | ForEach-Object { "  $_" }
            }
            # CASE B: Graphics Pipeline Shader Pass
            else
            {
                # 1. Compile Vertex Stage
                $vertArgs = @(
                    $shader.FullName,
                    "-entry", "vertexMain",
                    "-stage", "vertex",
                    "-target", "spirv",
                    "-D$($platform.define)",
                    "-o", "$outputLocation/$($shader.BaseName).vert.spv"
                )
                & slangc @vertArgs 2>&1 | ForEach-Object { "  $_" }

                # 2. Compile Fragment Stage (injects texture compression macros)
                $fragArgs = @(
                    $shader.FullName,
                    "-entry", "fragmentMain",
                    "-stage", "fragment",
                    "-target", "spirv",
                    "-D$($platform.define)",
                    "-D$($platform.textureFormat)",
                    "-o", "$outputLocation/$($shader.BaseName).frag.spv"
                )
                & slangc @fragArgs 2>&1 | ForEach-Object { "  $_" }
            }
        }
    }
}

cd $currentLocation
