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
        MAIN_DIR .. "/source/graphics/**.hpp"
    }

    includedirs
    {
        MAIN_DIR .. "/extern/ImGui/"
    }

    links
    {
        "d3d11",
        "dxgi",
        "d3dcompiler",
        "System",
        "DirectXTK",
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

    filter "configurations:Final"
        defines
        {
            "POP_RELEASE",
            "POP_FINAL"
        }

        runtime "Release"
        optimize "on"