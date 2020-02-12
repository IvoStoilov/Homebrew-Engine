include "premake_Global"

workspace "SystemSandbox"
    filename "SystemSandbox_%{_ACTION}"
    location(PROJECT_DIR)

    architecture "x64"
    startproject "Sandbox"

    configurations
    {
        "Debug",
        "Release"
    }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS",
        "_XM_NO_INTRINSICS_",
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

    characterset("ASCII");

project "Sandbox"
    location(PROJECT_DIR .. "/sandbox")
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir(OUTPUT_DIR)
    objdir(TEMP_DIR)

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

    filter "configurations:Debug"
        defines "POP_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "POP_RELEASE"
        runtime "Release"
        optimize "on"

-- **************
-- *** System ***
-- **************
    include "premake_System"