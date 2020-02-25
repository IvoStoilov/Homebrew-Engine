include "premake_GlobalVars"

workspace "SystemSandbox"
    filename "SystemSandbox_%{_ACTION}"
    location(PROJECT_DIR)

    architecture "x64"
    startproject "Sandbox"

    targetdir(OUTPUT_DIR .. "/sandbox")
    objdir(TEMP_DIR .. "/sandbox")
    characterset("ASCII");

    configurations
    {
        "Debug",
        "Release"
    }

    filter "configurations:Debug"
        targetsuffix "_d"
        defines "POP_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        targetsuffix "_r"
        defines "POP_RELEASE"
        runtime "Release"
        optimize "on"
    filter {} -- end filter "configurations:Release"

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

project "Sandbox"
    location(PROJECT_DIR .. "/sandbox")
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    files
    {
        MAIN_DIR .. "/source/sandbox/**.h",
        MAIN_DIR .. "/source/sandbox/**.cpp"
    }

    links
    {
        "System"
    }

    filter "system:windows"
		systemversion "latest"
		defines
		{
			"POP_PLATFORM_WINDOWS"
		}

-- **************
-- *** System ***
-- **************
 include "premake_System"
 project "System"
    targetdir(OUTPUT_DIR .. "/sandbox")
    objdir(TEMP_DIR .. "/sandbox")
