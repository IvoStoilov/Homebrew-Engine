group "Extern"
    include "premake_DirectXTK"
group ""

project "System"
    location(PROJECT_DIR .. "/system");
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    pchheader ("system/precompile.h")
    pchsource (MAIN_DIR .. "/source/system/precompile.cpp")

    files
    {
        MAIN_DIR .. "/source/system/**.h",
        MAIN_DIR .. "/source/system/**.cpp"
    }

    links
    {
        "DirectXTK"
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