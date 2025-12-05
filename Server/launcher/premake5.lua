project "Launcher"
	language "C++"
	kind "ConsoleApp"
	targetdir(buildpath("server"))
	targetname "project-monky-server"

	includedirs {
		"../../Shared/sdk",
		"../sdk",
	}

	vpaths {
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.cpp",
		["Resources/*"] = {"*.rc", "**.ico"},
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"*.h",
		"*.cpp"
	}

	filter "system:windows"
		targetname "Project Monky Server"
		staticruntime "On"
		files {
			"launcher.rc",
			"resource/mtaicon.ico"
		}

	filter "system:not windows"
		links { "dl" }
		buildoptions { "-pthread", "-fvisibility=default" }
		linkoptions { "-pthread", "-rdynamic" }

	filter "platforms:arm"
		targetname "project-monky-server-arm"

	filter "platforms:arm64"
		targetname "project-monky-server-arm64"

	filter { "system:linux", "platforms:x64" }
		targetname "project-monky-server"

	filter { "system:windows", "platforms:x64" }
		targetname "Project Monky Server"

	filter { "system:windows", "platforms:arm" }
		targetname "Project Monky Server ARM"

	filter { "system:windows", "platforms:arm64" }
		targetname "Project Monky Server ARM64"
