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

find_package(PNG REQUIRED)

add_executable(mir_gles_demo
	gl/Shader.h
	gl/Shader.cpp
	gl/Program.h
	gl/Program.cpp
	gl/ArrayBuffer.h
	gl/ArrayBuffer.cpp
	gl/Texture.h
	gl/Texture.cpp
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
	SwipeGesture.h
	SwipeGesture.cpp
	DemoRenderer.h
	DemoRenderer.cpp
	MirGLESDemo.cpp
)
target_compile_definitions(mir_gles_demo PRIVATE
	${PNG_DEFINITIONS}
	-D_USE_MATH_DEFINES
)
target_include_directories(mir_gles_demo SYSTEM PRIVATE
	${MIRCLIENT_INCLUDE_DIRS}
	${PNG_INCLUDE_DIRS}
	${GLM_INCLUDE_DIRS}
)
target_link_libraries(mir_gles_demo
	${MIRCLIENT_LIBRARIES}
	${LIBEGL}
	${LIBGLESv2}
	${PNG_LIBRARIES}
)
target_compile_options(mir_gles_demo PUBLIC ${MIRCLIENT_CFLAGS_OTHER})
set_target_properties(mir_gles_demo PROPERTIES LINK_FLAGS "${MIRCLIENT_LDFLAGS_OTHER}")

install(TARGETS mir_gles_demo RUNTIME DESTINATION .)
