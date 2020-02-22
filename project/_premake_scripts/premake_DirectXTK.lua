externalproject("DirectXTK")
    filename("DirectXTK_Desktop_2019_Win10")
    location(MAIN_DIR .. "/extern/DirectXTK/" )
    uuid "E0B52AE7-E160-4D32-BF3F-910B785E5A8E"
    kind "StaticLib"
    language "C++"

    --postbuildcommands
    --{
    --    "{COPY} %{prj.targetdir}/DirectXTK.lib %{wks.targetdir}"
    --}