project "Workspace"
	location "Workspace"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"
	systemversion "latest"

-- pchheader "etc/pch.h"
-- pchsource "%{wks.location}/%{prj.name}/etc/pch.cpp"

targetdir(binDir)
objdir(binIntDir)

debugdir("%{WorkingDir.Root}/Resource")

files
{
	"%{prj.name}/**.cpp",
	"%{prj.name}/**.hpp",
	"%{prj.name}/**.h",
}

includedirs
{
	"%{IncludeDir.Workspace}/",
	"%{IncludeDir.VegaEngine2}/src/",
	"%{IncludeDir.SFML}/",
	"%{IncludeDir.Spdlog}/",
	"%{IncludeDir.ImGui}/",
	"%{IncludeDir.Entt}/",
	"%{IncludeDir.Box2D}/",
}

libdirs
{
	"%{LibraryDir.VegaEngine2}/%{cfg.buildcfg}/",
	"%{LibraryDir.SFML}/%{cfg.buildcfg}/",
	"%{LibraryDir.ImGui}/%{cfg.buildcfg}/",
	"%{LibraryDir.Box2D}/%{cfg.buildcfg}/",
}

links
{
	"VegaEngine2",
	"VegaEngine2.lib",
	"sfml-audio.lib",
	"sfml-graphics.lib",
	"sfml-main.lib",
	"sfml-network.lib",
	"sfml-system.lib",
	"sfml-window.lib",
	"ImGui.lib",
	"opengl32",
	"box2d.lib",
}

prebuildcommands 
{ 
	
}

filter "system:Windows"
linkoptions { "/IGNORE:4006" }
buildoptions { "/bigobj" }

defines
{
	"VG_PLATFORM_WINDOWS",
}

filter "configurations:Debug"
	defines "VG_DEBUG"
	runtime "Debug"
	symbols "On"

filter "configurations:Release"
	defines "VG_RELEASE"
	runtime "Release"
	optimize "On"
