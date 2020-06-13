group "Extern"
    include "p_premake_DirectXTK"
group ""

include "p_premake_System"

project "Graphics"
    location(PROJECT_DIR .. "/graphics");
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    
    pchheader ("graphics/precompile.h")
    pchsource (MAIN_DIR .. "/source/graphics/precompile.cpp")

    files
    {
        MAIN_DIR .. "/source/graphics/**.h",
        MAIN_DIR .. "/source/graphics/**.cpp",
        MAIN_DIR .. "/source/graphics/**.hpp",
        MAIN_DIR .. "/source/graphics/**.hlsl"
    }

    includedirs
    {
        MAIN_DIR .. "/extern/ImGui/",
        MAIN_DIR .. "/extern/assimp/assimp-repo/include/"
    }

    links
    {
        "d3d11",
        "dxgi",
        "d3dcompiler",
        "System",
        "DirectXTK",
    }

    libdirs
    {
        MAIN_DIR .. "/extern/assimp/assimp-lib"
    }

    filter "system:windows"
        systemversion "latest"
        defines
        {
            "POP_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines "POP_DEBUG"
        runtime "Debug"
        symbols "on"
        links
        {
            "zlibstaticd",
            "IrrXMLd",
            "assimp-vc142-mtd"
        }

    filter "configurations:Release"
        defines "POP_RELEASE"
        runtime "Release"
        optimize "on"
        links
        {
            "zlibstatic",
            "IrrXML",
            "assimp-vc142-mt"
        }

    filter "configurations:Final"
        defines
        {
            "POP_RELEASE",
            "POP_FINAL"
        }

        runtime "Release"
        optimize "on"
    filter {} --filter "configurations:Final"

    
    shadermodel "5.0"
    -- Warnings as errors
    --shaderoptions({"/WX"}) - Commented because of X4000: use of potentially uninitialized variable, occuring from a mid function, function call

    --shaderassembler("AssemblyCode")

    -- HLSL files that don't end with 'Extensions' will be ignored
    -- as they will be used as includes
    filter ("files:**.hlsl")
        flags("ExcludeFromBuild")
        local shader_dir = OUTPUT_DIR .. "/shader_bin"
        shaderobjectfileoutput(shader_dir.."/%{file.basename}"..".cso")
        shaderassembleroutput(shader_dir.."/%{file.basename}"..".asm")
        --This is not needed as VS automatically sets the options and complains that 
        --options are set twice
        --shaderoptions
        --{
        --    "/Od", --Disable Optimizations
        --    "/Zi"  --Enable Debug Information
        --}

   filter("files:**PS.hlsl")
      removeflags("ExcludeFromBuild")
      shadertype("Pixel")

   filter("files:**VS.hlsl")
      removeflags("ExcludeFromBuild")
      shadertype("Vertex")