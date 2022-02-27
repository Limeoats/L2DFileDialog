workspace "L2DFileDialog"
    architecture "x64"
    startproject "L2DFileDialog"

    configurations 
    {
        "Debug",
        "Release",
        "Dist"
    }

    flags
	{
		"MultiProcessorCompile"
	}

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    -- Include directories relative to root folder (solution directory)
    IncludeDir = {}
    IncludeDir["SDL2"] = "L2DFileDialog/lib/SDL2/include"
    IncludeDir["imgui"] = "L2DFileDialog/lib/imgui"
    IncludeDir["vendor"] = "Lime2D/lib"

    libdirs
	{ 
		"L2DFileDialog/lib/SDL2/lib"
	}


    group "Dependencies"

    group ""

    project "L2DFileDialog"
        location "L2DFileDialog"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++17"
        staticruntime "on"

        targetdir ("bin/" .. outputdir .. "/%{prj.name}")
        objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

        files
        {
            "%{prj.name}/src/**.cpp",
            "%{prj.name}/src/**.h",
            "%{prj.name}/lib/imgui/*.cpp",
            "%{prj.name}/lib/imgui/backends/imgui_impl_opengl3.cpp",
            "%{prj.name}/lib/imgui/backends/imgui_impl_sdl.cpp"
        }

        includedirs
        {
            "%{prj.name}/include",
            "%{IncludeDir.SDL2}",
            "%{IncludeDir.imgui}",
            "%{IncludeDir.imgui}/backends"
        }
        
        links {
            "SDL2",
            "opengl32.lib"
        }

        

        filter "system:windows"
            systemversion "latest"

            defines
            {
                "L2D_PLATFORM_WINDOWS",
                "L2D_BUILD_DLL"
            }

        filter "configurations:Debug"
            defines "L2D_DEBUG"
            runtime "Debug"
            symbols "on"
        
        filter "configurations:Release"
            defines "L2D_RELEASE"
            runtime "Release"
            optimize "on"

        filter "configurations:Dist"
            defines "L2D_DIST"
            runtime "Release"
            optimize "on"