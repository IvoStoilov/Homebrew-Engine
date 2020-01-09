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