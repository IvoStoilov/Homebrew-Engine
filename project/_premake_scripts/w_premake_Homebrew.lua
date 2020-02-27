include "g_premake_GlobalVars"

workspace "HomebrewEngine"
    filename "HomebrewEngine_%{_ACTION}"
    location(PROJECT_DIR)

    architecture "x64"
    startproject "Gameplay"

    targetdir(OUTPUT_DIR .. "/homebrew")
    objdir(TEMP_DIR .. "/homebrew")
    characterset("ASCII")

    configurations
    {
        "Debug",
        "Release",
        "Final"
    }

    filter "configurations:Debug"
        targetsuffix "_d"
        defines "POP_DEBUG"
        runtime "Debug"
        symbols "on"
    filter {} --filter "configurations:Debug"

    filter "configurations:Release"
        targetsuffix "_r"
        defines "POP_RELEASE"
        runtime "Release"
        optimize "on"
    filter {} -- filter "configurations:Release"

    filter "configurations:Final"
        targetsuffix "_f"
        defines
        {
            "POP_RELEASE",
            "POP_FINAL"
        }
        runtime "Release"
        optimize "on"
    filter {} --filter "configurations:Final"

    filter "system:windows"
		systemversion "latest"
		defines
		{
			"POP_PLATFORM_WINDOWS"
        }
    filter {} --filter "system:windows"

    defines
    {
        "_CRT_SECURE_NO_WARNINGS",
    }

    includedirs
    {
        MAIN_DIR,
        MAIN_DIR .. "/source"
    }

    disablewarnings 
    {
        "4005",
        "4530",
        "4577",
        "4996"
    }

group "Extern"
    include "p_premake_DirectXTK"
group ""

-- ****************
-- *** Gameplay ***
-- ****************
project "Gameplay"
    location(PROJECT_DIR .. "/gameplay")
    kind "WindowedApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    files
    {
        MAIN_DIR .. "/source/gameplay/**.h",
        MAIN_DIR .. "/source/gameplay/**.cpp",
    }

    links
    {
        "Engine"
    }

project "Engine"
    location(PROJECT_DIR .. "/engine")
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    
    --pchheader precompile
    --pchsource source/ ..... / precompile.cpp

    files
    {
        MAIN_DIR .. "/source/engine/**.h",
        MAIN_DIR .. "/source/engine/**.cpp",
    }

    links 
    {
        "Graphics", 
        "System"
    }

    include "p_premake_Graphics"
    include "p_premake_System"