version = "1.9.24"

workspace "Solution"
	startproject "Vega-Editor"
	architecture "x86_64"

	configurations
	{
		"Debug",
		"Release",
	}

	flags
	{
		"MultiProcessorCompile"
	}

	outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}"
	binDir = "%{wks.location}/Build/bin/" .. outputDir
	binIntDir = "%{wks.location}/Build/bin-int/" .. outputDir

	-- Include Directories
	IncludeDir = {}
	IncludeDir["SFML"]			= "%{wks.location}/Vendor/SFML-2.6.1/include"
	IncludeDir["Spdlog"]		= "%{wks.location}/Vendor/spdlog/include/" 
	IncludeDir["VegaEngine2"]	= "%{wks.location}/VegaEngine2/VegaEngine2"
	IncludeDir["Vega-Editor"]	= "%{wks.location}/Vega-Editor/Vega-Editor"
	IncludeDir["Workspace"]		= "%{wks.location}/Workspace/Workspace"
	IncludeDir["ImGui"]			= "%{wks.location}/Vendor/ImGui/"
	IncludeDir["Entt"]			= "%{wks.location}/Vendor/Entt/include/"
	IncludeDir["Box2D"]			= "%{wks.location}/Vendor/Box2D/include"

	-- Library Path
	LibraryDir = {}
	LibraryDir["VegaEngine2"]	= "%{wks.location}/Vendor/VegaEngine2-"..version.."/lib"
	LibraryDir["SFML"]			= "%{wks.location}/Vendor/SFML-2.6.1/lib"
	LibraryDir["ImGui"]			= "%{wks.location}/Vendor/ImGui/lib"
	LibraryDir["Box2D"]			= "%{wks.location}/Vendor/Box2D/lib"

	-- DLL Working Directories
	WorkingDir = {}
	WorkingDir["Root"]			= "%{wks.location}"
	WorkingDir["SFML"]			= "%{wks.location}/Vendor/SFML-2.6.1/bin"

	group "Dependencies"
		include "Vendor/ImGui"
		include "Vendor/premake5"
	group ""

include "VegaEngine2"
include "Vega-Editor"
include "Workspace"

