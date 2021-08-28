workspace "OpenGLSandbox"
    architecture "x64"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
    startproject "OpenGLSandbox"

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
    
-- Example : Debug/Windows/x64
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Additional Projects(static libs) to link to
group "Dependencies"
	include "OpenGLSandbox/vendor/GLFW"
    include "OpenGLSandbox/vendor/ASSIMP/IrrXML"
    include "OpenGLSandbox/vendor/ASSIMP/zlibstatic"
    include "OpenGLSandbox/vendor/ASSIMP"
    include "OpenGLSandbox/vendor/Glad"
	include "OpenGLSandbox/vendor/imgui"
group ""

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
        "%{prj.name}/src/**.hpp",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/res/**.shader",
        "%{prj.name}/res/**.png",
        "%{prj.name}/res/**.obj",
        "%{prj.name}/res/**.mtl",
        "%{prj.name}/res/**.jpg",
        "%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
        "%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl"
    }

-- C/C++ -> Preprocessor -> Preprocessor Definitions
	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

-- C/C++ -> General -> Additional Include Directories
    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor",
        "%{prj.name}/vendor/GLFW/include",
        "%{prj.name}/vendor/ASSIMP/include",
        "%{prj.name}/vendor/Glad/include",
        "%{prj.name}/vendor/imgui",
        "%{prj.name}/vendor/glm"
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
        "GLFW",
        "zlibstatic",
        "IrrXML",
        "ASSIMP",
        "Glad",
        "ImGui",
        "opengl32.lib"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"
    
    filter "configurations:Debug"
        -- c/c++ -> code generation -> runtime library : multi-threaded debug dll (/MDd)    
        -- buildoptions "/MDd"
        symbols "On"
    
    filter "configurations:Release"
        optimize "On"

    filter "configurations:Dist"
        optimize "On"