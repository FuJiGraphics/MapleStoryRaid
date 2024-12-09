project "VegaEngine2"
	location "VegaEngine2"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"
	systemversion "latest"

pchheader "pch.h"
pchsource "%{IncludeDir.VegaEngine2}/src/pch.cpp"

targetdir("%{wks.location}/Vendor/%{prj.name}-"..version.."/lib/%{cfg.buildcfg}/")
objdir("%{wks.location}/Vendor/%{prj.name}-"..version.."/bin-int/%{cfg.buildcfg}/")

debugdir("%{WorkingDir.Root}/Resource")

files
{
	"%{prj.name}/**.cpp",
	"%{prj.name}/**.hpp",
	"%{prj.name}/**.h",
}

includedirs
{
	"%{IncludeDir.VegaEngine2}/src/",
	"%{IncludeDir.Workspace}/",
	"%{IncludeDir.SFML}/",
	"%{IncludeDir.Spdlog}/",
	"%{IncludeDir.ImGui}/",
	"%{IncludeDir.Entt}/",
	"%{IncludeDir.Box2D}/",
}

libdirs
{
	"%{LibraryDir.SFML}/%{cfg.buildcfg}/",
	"%{LibraryDir.ImGui}/%{cfg.buildcfg}/",
	"%{LibraryDir.Box2D}/%{cfg.buildcfg}/",
}

prebuildcommands 
{
    "mkdir \"%{wks.location}\\Vendor\\%{prj.name}-"..version.."\\include\\%{prj.name}\"",
    "xcopy /Y /S \"%{IncludeDir.VegaEngine2}\\*.h\" \"%{wks.location}Vendor\\%{prj.name}-"..version.."\\include\\%{prj.name}\"",
    "xcopy /Y /S \"%{IncludeDir.VegaEngine2}\\*.hpp\" \"%{wks.location}Vendor\\%{prj.name}-"..version.."\\include\\%{prj.name}\""
}

links
{
	"sfml-audio.lib",
	"sfml-graphics.lib",
	"sfml-main.lib",
	"sfml-network.lib",
	"sfml-system.lib",
	"sfml-window.lib",
	"ImGui.lib",
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
