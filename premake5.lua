workspace "UnlimitedForest"
  configurations { "Debug", "Release" }
  platforms      { "x64", "x86" }
  startproject   "UnlimitedForest"

  -- Add this to integrate vcpkg with premake
  if os.getenv("VCPKG_ROOT") then
    print("Using vcpkg integration from: " .. os.getenv("VCPKG_ROOT"))
  else
    print("WARNING: VCPKG_ROOT environment variable not set!")
  end

project "UnlimitedForest"
  kind       "ConsoleApp"
  language   "C++"
  cppdialect "C++20"
  targetdir  "bin/%{cfg.buildcfg}/%{cfg.platform}"

  -- Wildcard include for all source, headers, and shaders
  files {
    "UnlimitedForest/UnlimitedForest.cpp",
    "UnlimitedForest/UnlimitedForest.h",
    "UnlimitedForest/glad.c",
    "UnlimitedForest/core/**.cpp",
    "UnlimitedForest/core/**.h",
    "UnlimitedForest/core/**.hpp",
    "UnlimitedForest/shaders/**.*",
    "UnlimitedForest/external/glad/**.h",
    "UnlimitedForest/external/glm/**.hpp",
    "UnlimitedForest/external/glm/**.inl",
    "external/tracy/public/tracy/**.cpp",
    "external/tracy/public/tracy/**.hpp",
  }

  -- Dynamically generate VS filters for each subfolder under core/
  filter {}
  local coreBase = "UnlimitedForest/core"
  for _, dir in ipairs(os.matchdirs(coreBase .. "/*")) do
    local name = path.getname(dir)
    vpaths { ["Core/" .. name] = { dir .. "/**.*" } }
  end

  includedirs {
    "UnlimitedForest",
    "UnlimitedForest/core",
    "UnlimitedForest/external/glad/include",
    "UnlimitedForest/external/glm",
    "external/tracy/public",
    "$(VCPKG_ROOT)/installed/x64-windows/include",
  }

  libdirs {
    "$(VCPKG_ROOT)/installed/x64-windows/lib",
    "$(VCPKG_ROOT)/installed/x64-windows/debug/lib",
  }

  links {
    "OpenGL32",
  }

  filter "configurations:Debug"
    defines { "DEBUG", "TRACY_ENABLE" }
    symbols "On"
    links { "SDL2d", "SDL2maind", "spdlogd" }

  filter "configurations:Release"
    defines "NDEBUG"
    optimize "On"
    links { "SDL2", "SDL2main", "spdlog" }

  filter { "system:windows" }
    buildoptions { "/utf-8" }
    postbuildcommands {
      "{COPY} \"$(VCPKG_ROOT)/installed/x64-windows/bin/SDL2.dll\" \"%{cfg.targetdir}/\"",
      "{COPY} \"$(VCPKG_ROOT)/installed/x64-windows/bin/spdlog.dll\" \"%{cfg.targetdir}/\"",
      "{COPY} \"UnlimitedForest/shaders\" \"%{cfg.targetdir}/shaders/\""
    }

  filter { "system:windows", "configurations:Debug" }
    postbuildcommands {
      "{COPY} \"$(VCPKG_ROOT)/installed/x64-windows/debug/bin/SDL2d.dll\" \"%{cfg.targetdir}/\"",
      "{COPY} \"$(VCPKG_ROOT)/installed/x64-windows/debug/bin/spdlogd.dll\" \"%{cfg.targetdir}/\"",
      "{COPY} \"UnlimitedForest/shaders\" \"%{cfg.targetdir}/shaders/\""
    }
