NAME = (io.popen"cd":read'*l'):match("[^/\\]*$") -- current folder name

workspace (NAME)
   configurations { "Debug", "Release" }

project (NAME)
	kind "ConsoleApp"
	language "C++"
	architecture "x64"
	cppdialect "C++20"
	toolset "clang"
	targetdir "bin/%{cfg.buildcfg}"
	defines{"WIN32", "_CONSOLE"}

	files { 
		"src/**.h", "src/**.hpp",
		"src/**.c", "src/**.cpp"
	}

	-- DEPENDENCIES

	includedirs { "deps/include" }

	libdirs{}

	links{}

	-- add your dlls here to be copied to build folder:
	--postbuildcommands {
	--	"{COPY} pathtodll/file.dll %{cfg.targetdir}"
	--}

	buildoptions{
		--"-openmp",
		--"-verbose"
	}

	filter "configurations:Debug"
		defines{"_DEBUG"}
		symbols "On"

	filter "configurations:Release"
		defines {"NDEBUG"}
		optimize "On"