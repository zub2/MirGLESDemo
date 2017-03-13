find_package(PkgConfig)
pkg_check_modules(MIRCLIENT REQUIRED mirclient)

find_library(LIBEGL EGL)
if (NOT LIBEGL)
	message(FATAL_ERROR "Can't find libEGL")
endif()

find_library(LIBGLESv2 GLESv2)
if (NOT LIBGLESv2)
	message(FATAL_ERROR "Can't find libGLESv2")
endif()

# Currently the Ubuntu SDK target containers lack boost devel packages.
# In order for the following command to succeed it's necessary
# to install at least libboost-filesystem-dev and libboost-system-dev
# for the target arch. For example, for armhf, run:
# $ usdk-target maint device-armhf apt install libboost-filesystem-dev:armhf libboost-system-dev:armhf
find_package(Boost REQUIRED COMPONENTS filesystem system)

find_package(PNG REQUIRED)

add_executable(mir_gles_demo
	gl/Shader.h
	gl/Shader.cpp
	gl/Program.h
	gl/Program.cpp
	Exceptions.h
	MirConnectionWrapper.h
	MirConnectionWrapper.cpp
	MirNativeWindowRenderer.h
	MirNativeWindowControl.h
	MirNativeWindow.h
	MirNativeWindow.cpp
	ResourcePath.h
	ResourcePath.cpp
	ShaderLoader.h
	ShaderLoader.cpp
	Image.h
	Image.cpp
	PNGLoader.h
	PNGLoader.cpp
	DemoRenderer.h
	DemoRenderer.cpp
	MirGLESDemo.cpp
)
target_compile_definitions(mir_gles_demo PRIVATE ${PNG_DEFINITIONS})
target_include_directories(mir_gles_demo SYSTEM PRIVATE
	${MIRCLIENT_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${PNG_INCLUDE_DIRS}
	${GLM_INCLUDE_DIRS}
)
target_link_libraries(mir_gles_demo
	${MIRCLIENT_LIBRARIES}
	${LIBEGL}
	${LIBGLESv2}
	${Boost_LIBRARIES}
	${PNG_LIBRARIES}
)
target_compile_options(mir_gles_demo PUBLIC ${MIRCLIENT_CFLAGS_OTHER})
set_target_properties(mir_gles_demo PROPERTIES LINK_FLAGS "${MIRCLIENT_LDFLAGS_OTHER}")

install(TARGETS mir_gles_demo RUNTIME DESTINATION .)
