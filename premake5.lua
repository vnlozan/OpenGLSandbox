workspace "OpenGLSandbox"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

-- Example : Debug/Windows/x64
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "OpenGLSandbox"
    location "OpenGLSandbox"
    kind "ConsoleApp"
    language "C++"

    targetdir ( "bin/" .. outputdir .. "/%{prj.name}" )
    objdir ( "bin-int/" .. outputdir .. "/%{prj.name}" )

-- All file extensions used in project
    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/res/**.shader",
        "%{prj.name}/res/**.png"
    }

-- C/C++ -> General -> Additional Include Directories
    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor",
        "%{prj.name}/vendor/GLFW/include",
        -- "vendor/GLEW/include",
    }

-- Linker -> General -> Additional Library Directories
    libdirs
    {
        -- "vendor/GLFW/lib-vc2019",
        -- "vendor/GLEW/lib/Release/x64"
    }

-- Linker -> Input -> Additional Dependencies
    links 
    {
        -- "glew32s",
        "libglew_static"
        "GLFW",
        "ImGui",
        "opengl32.lib",
        -- "user32",
        -- "gdi32",
        -- "Shell32"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        
-- C/C++ -> Preprocessor -> Preprocessor Definitions
        defines
        {
            "GLEW_STATIC",
            "_DEBUG",
            "_CONSOLE"
        }
    
    filter "configurations:Debug"
        -- c/c++ -> code generation -> runtime library : multi-threaded debug dll (/MDd)    
        buildoptions "/MDd"
        symbols "On"
    
    filter "configurations:Release"
        buildoptions "/MD"
        optimize "On"

    filter "configurations:Dist"
        buildoptions "/MD"    
        optimize "On"