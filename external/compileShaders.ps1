$currentLocation            =  "$PWD"
$shaderSourceLocation       =  "$HOME/CLionProjects/AjaxTown/assets/shaders/source"
$shaderDestinationLocation  =  "$HOME/CLionProjects/AjaxTown/assets/shaders/compiled"

# not need for -I$shaderSourceLocation for header as we are already in that folder so glslc will search for it automatically
cd $shaderSourceLocation

#echo $PWD
$shaders = dir $shaderSourceLocation -Recurse -Include *.vert, *.frag, *.comp
$headers = dir $shaderSourceLocation -Recurse -Include *.glsl

$platforms = @(
                @{ name = "android"; define = "TARGET_PLATFORM_ANDROID" },
                @{ name = "win32";   define = "TARGET_PLATFORM_WIN32"   }
              )

foreach($platform in $platforms)
{
    $outputLocation = "$shaderDestinationLocation/$($platform.name)"

    #create folders
    if(! (Test-Path ($outputLocation) ))
    {
        mkDir $outputLocation | Out-Null
    }

    foreach($shader in $shaders)
    {
        $spv = dir "$outputLocation/$($Shader.Name).spv" 2>$null

        if($spv -eq $null -or $shader.LastWriteTime -gt $spv.LastWriteTime -or ($headers | Where-Object{$_.LastWriteTime -gt $spv.LastWriteTime}))
        {
            #echo "Compiling [$($platform.name)]: $($shader.Name)"
            #echo "CMD: glslc $($shader.Name) -D$($platform.define) --target-env=vulkan1.3 -o $outputLocation/$($shader.Name).spv"
            #glslc $shader.Name -D$($platform.define)  --target-env=vulkan1.3 -o "$outputLocation/$($shader.Name).spv" 2>&1 | ForEach-Object { echo "  $_" }

            $glslcArgs = @(
                $shader.FullName,                              # full path — removes cd dependency entirely
                "-D$($platform.define)",                       # quoted — prevents PS parsing -D as a switch
                "--target-env=vulkan1.3",
                "-o",                                          # -o and path as separate elements
                "$outputLocation/$($shader.Name).spv"
            )

            echo "Compiling [$($platform.name)]: $($shader.Name)"
            & glslc @glslcArgs 2>&1 | ForEach-Object { echo "  $_" }
        }
    }

}



#robocopy $shaderSourceLocation $ShaderDestinationLocation /E /XO /XF *.vert *.frag *.comp
#$compiledShaders = dir $shaderSourceLocation -Recurse -Include *.spv
#
#foreach($compiledShader in $compiledShaders)
#{
#    #echo $compiledShader.Name
#}

cd $currentLocation

#--target-env=vulkan1.3