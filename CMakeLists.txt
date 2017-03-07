project(MirGLESDemo)
cmake_minimum_required(VERSION 3.0)

# Fix Ubuntu SDK IDE messing up C++ compilation...
string(REGEX REPLACE "gcc$" "g++" CMAKE_CXX_COMPILER "${CMAKE_CXX_COMPILER}")

# Set C++ standard. The SDK containers currently have CMake 3.0.2 which
# does not support target property CXX_STANDARD and friends. So just pass
# it as compile option.
add_compile_options(-std=c++11)

# enable some warnings
add_compile_options(-Wall -Wextra -pedantic)

# Do not remove this line, its required for the correct functionality of the Ubuntu-SDK
set(UBUNTU_MANIFEST_PATH "manifest.json.in" CACHE INTERNAL "Tells QtCreator location and name of the manifest file")

# Set install paths
set(CMAKE_INSTALL_PREFIX /)
set(DATA_DIR .)

# This command figures out the target architecture for use in the manifest file
execute_process(
	COMMAND dpkg-architecture -qDEB_HOST_ARCH
	OUTPUT_VARIABLE CLICK_ARCH
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

configure_file(manifest.json.in ${CMAKE_CURRENT_BINARY_DIR}/manifest.json)
install(FILES
	${CMAKE_CURRENT_BINARY_DIR}/manifest.json
	MirGLESDemo.desktop
	MirGLESDemo.apparmor
	DESTINATION .
)
install(DIRECTORY "media" DESTINATION ${DATA_DIR} PATTERN "media/src" EXCLUDE)

add_subdirectory(src)

# No op custom target for all not compiled files, so they show up in the QtCreator project tree
add_custom_target("MirGLESDemo_ClickFiles" ALL SOURCES "MirGLESDemo.desktop" "MirGLESDemo.apparmor" "manifest.json.in")
