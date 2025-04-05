# CMakeList.txt : CMake project for UnlimitedForest, include source and define
# project specific logic here.

# Source files
set(SOURCES
	"UnlimitedForest.cpp"
	"UnlimitedForest.h"
	"glad.c"
)

# Add source to this project's executable.
add_executable (UnlimitedForest ${SOURCES})

# SDL2 and OpenGL paths
set(SDL2_DIR "C:/libs/SDL2")
set(SDL2_INCLUDE_DIR "${SDL2_DIR}/include")
set(SDL2_LIB_DIR "${SDL2_DIR}/lib/x64")

# Include SDL2 and  headers
include_directories(
    ${SDL2_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/glad/include
)
# Link directories for SDL2
link_directories(${SDL2_LIB_DIR})

# Find OpenGL
find_package(OpenGL REQUIRED)

# Link SDL2 and OpenGL
target_link_libraries(UnlimitedForest
	${SDL2_LIB_DIR}/SDL2.lib
	${SDL2_LIB_DIR}/SDL2main.lib
	OpenGL::GL
)


# Copy SDL2.dll to build output folder after build
add_custom_command(TARGET UnlimitedForest POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${SDL2_LIB_DIR}/SDL2.dll"
	$<TARGET_FILE_DIR:UnlimitedForest>
)
