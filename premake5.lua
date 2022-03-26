
workspace "DefEngine"
   configurations { "Debug", "Release" }

	libdirs { "dependencies/glfw/lib", "dependencies/OpenAL/lib" }
	links {"glfw3", "mingw32", "OpenAL32", "opengl32", "gdi32"}
	includedirs { "include", "dependencies/glfw/include", "dependencies/OpenAL/include", "dependencies/glad/include", "dependencies", "dependencies/stb/" }
	buildoptions { "-g", "-O0", "-w"}
	
project "DefEngine"
   -- kind "WindowedApp"
   kind "ConsoleApp"
   language "C++"
	
   files { "*.h", "*.cpp", "glad.cpp"}
   targetname ("EngineTest")
   targetdir ("")

   filter "configurations:Debug"
      symbols "On"

   filter "configurations:Release"
      optimize "On"