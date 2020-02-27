include "g_premake_GlobalVars"

workspace "SystemSandbox"
    filename "SystemSandbox_%{_ACTION}"
    location(PROJECT_DIR)

    architecture "x64"
    startproject "Sandbox"

    targetdir(OUTPUT_DIR .. "/system_sandbox")
    objdir(TEMP_DIR .. "/system_sandbox")
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
    filter {} --filter "configurations:Debug"

    filter "configurations:Release"
        targetsuffix "_r"
        defines "POP_RELEASE"
        runtime "Release"
        optimize "on"
    filter {} --filter "configurations:Release"

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

project "Sandbox"
    location(PROJECT_DIR .. "/system_sandbox")
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    files
    {
        MAIN_DIR .. "/source/sandbox/system/**.h",
        MAIN_DIR .. "/source/sandbox/system/**.cpp"
    }

    links
    {
        "System"
    }

 include "p_premake_System"