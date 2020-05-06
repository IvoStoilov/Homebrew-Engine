include "g_premake_GlobalVars"

workspace "GraphicsSandbox"
    filename "GraphicsSandbox_%{_ACTION}"
    location(PROJECT_DIR)

    architecture "x64"
    startproject "Sandbox"

    targetdir(OUTPUT_DIR .. "/graphics_sandbox")
    objdir(TEMP_DIR .. "/graphics_sandbox")
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
        MAIN_DIR .. "/source",
        MAIN_DIR .. "/extern/spdlog/include/"
    }

    disablewarnings 
    {
        "4005",
        "4530",
        "4577",
        "4996"
    }

    project "Sandbox"
        location(PROJECT_DIR .. "/graphics_sandbox")
        debugdir(OUTPUT_DIR .. "/graphics_sandbox")
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++17"

        files
        {
            MAIN_DIR .. "/source/sandbox/graphics/**.h",
            MAIN_DIR .. "/source/sandbox/graphics/**.cpp"
        }

        links
        {
            "Graphics",
            "System"
        }

    include "p_premake_System"
    include "p_premake_Graphics"