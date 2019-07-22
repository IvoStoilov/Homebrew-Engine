-- *****************
-- *** Constants ***
-- *****************
PROJECT_DIR = "../"
MAIN_DIR    = "../.."
OUTPUT_DIR  = MAIN_DIR .. "/bin/%{cfg.buildcfg}"
TEMP_DIR    = MAIN_DIR .. "temp/%{cfg.buildcfg}"

workspace "HomebrewEngine"
	filename "HomebrewEngine_%{_ACTION}"
	location(PROJECT_DIR)

    architecture "x64"
    startproject "Gameplay"
    
    configurations
    {
        "Debug",
        "Release",
        "Final"
    }

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"_XM_NO_INTRINSICS_"
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
	include "premake_DirectXTK"
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

 	targetdir(OUTPUT_DIR);
    objdir(TEMP_DIR);
    
	files
    {
        MAIN_DIR .. "/source/gameplay/**.h",
        MAIN_DIR .. "/source/gameplay/**.cpp",
    }

	links
	{
		"Engine"
	}

	filter "system:windows"
		systemversion "latest"

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

project "Engine"
    location(PROJECT_DIR .. "/engine")
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    
    targetdir(OUTPUT_DIR);
    objdir(TEMP_DIR);

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

    includedirs
    {
        os.getenv("DXSDK_DIR") .. "/Include"
    }
    
	filter "system:windows"
		systemversion "latest"

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

-- ****************
-- *** Graphics ***
-- ****************
project "Graphics"
    location(PROJECT_DIR .. "/graphics");
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    
    targetdir(OUTPUT_DIR);
    objdir(TEMP_DIR);

    links
    {
    	"System"
    }
    --pchheader precompile
    --pchsource source/ ..... / precompile.cpp

    files
    {
        MAIN_DIR .. "/source/graphics/**.h",
        MAIN_DIR .. "/source/graphics/**.cpp",
    }

    includedirs
    {
        os.getenv("DXSDK_DIR") .. "/Include"
    }

    links
    {
    	"System",
    	"DirectXTK",
    	"d3dx11",
    	"d3dx10"
    }

    libdirs
    {
    	os.getenv("DXSDK_DIR") .. "/Lib/x64",
    	OUTPUT_DIR -- because it is missing the xcopy libs
	}

	filter "system:windows"
		systemversion "latest"

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

-- **************
-- *** System ***
-- **************
project "System"
    location(PROJECT_DIR .. "/system");
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    
    targetdir(OUTPUT_DIR);
    objdir(TEMP_DIR);

    --pchheader precompile
    --pchsource source/ ..... / precompile.cpp

    files
    {
        MAIN_DIR .. "/source/system/**.h",
        MAIN_DIR .. "/source/system/**.cpp"
    }

	includedirs
    {
        os.getenv("DXSDK_DIR") .. "/Include"
    }

	libdirs
    {
    	os.getenv("DXSDK_DIR") .. "/Lib/x64"
    }

	filter "system:windows"
		systemversion "latest"

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