/*
 * Copyright 2017 David Kozub <zub at linux.fjfi.cvut.cz>
 *
 * This file is part of MirGLESDemo.
 *
 * MirGLESDemo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MirGLESDemo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MirGLESDemo.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "MirNativeWindow.h"
#include "Exceptions.h"

#include <utility>
#include <boost/thread/lock_guard.hpp>
#include <iostream>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

MirNativeWindow::MirNativeWindow(MirConnection* connection, int outputId, const char* surfaceName, std::shared_ptr<MirNativeWindowRenderer> renderer):
	m_connection(connection),
	m_surface(nullptr, mir_surface_release_sync),
	m_width(0),
	m_height(0),
	m_eglDisplay(EGL_NO_DISPLAY),
	m_eglSurface(EGL_NO_SURFACE),
	m_renderer(std::move(renderer))
{
	if (!m_connection)
		throw std::runtime_error("MirNativeWindow::MirNativeWindow: created with null mir connection!");

	initEGL();

	EGLConfig eglConfig = getEglConfig();
	const MirPixelFormat pixelFormat = mir_connection_get_egl_pixel_format(m_connection, m_eglDisplay, eglConfig);

	createFullScreenSurface(surfaceName, outputId, pixelFormat);

	// FIXME: this is deprecated
	MirBufferStream *bufferStream = mir_surface_get_buffer_stream(m_surface.get());

	/*
	 * EGLNativeWindowType is a typedef to (X11) Wisplay even though it's in fact MirEGLNativeWindowType with Mir
	 * but this is nothing a little reinterpret_cast couldn't fix...
	 */
	EGLNativeWindowType eglNativeWindow = reinterpret_cast<EGLNativeWindowType>(mir_buffer_stream_get_egl_native_window(bufferStream));

	m_eglSurface = eglCreateWindowSurface(m_eglDisplay, eglConfig, eglNativeWindow, nullptr);
	if (m_eglSurface == EGL_NO_SURFACE)
		throw std::runtime_error("Can't create EGL surface!");

	if (eglBindAPI(EGL_OPENGL_ES_API) != EGL_TRUE)
		throw EGLError("Can't bind OpenGL ES API!");

	const EGLint contextAttribList[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};
	EGLContext eglContext = eglCreateContext(m_eglDisplay, eglConfig, EGL_NO_CONTEXT, contextAttribList);
	if (eglContext == EGL_NO_CONTEXT)
		throw EGLError("Can't create EGL context!");

	if (eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, eglContext) != EGL_TRUE)
		throw EGLError("Can't make context current!");
}

MirNativeWindow::~MirNativeWindow()
{
	eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglTerminate(m_eglDisplay);
}

void MirNativeWindow::run()
{
	m_renderer->run(*this);
}

int MirNativeWindow::getWidth()
{
	return m_width;
}

int MirNativeWindow::getHeight()
{
	return m_height;
}

void MirNativeWindow::swapBuffers()
{
	eglSwapBuffers(m_eglDisplay, m_eglSurface);
}

void MirNativeWindow::surfaceEventHandler(const MirEvent* event)
{
	boost::lock_guard<boost::mutex> guard(m_eventMutex);
	m_renderer->handleEvent(event);
}

void MirNativeWindow::initEGL()
{
	/*
	 * EGLNativeDisplayType is a typedef to XDisplay even though it's in fact MirEGLNativeDisplayType with Mir
	 * but this is nothing a little reinterpret_cast couldn't fix...
	 */
	m_eglDisplay = eglGetDisplay(reinterpret_cast<EGLNativeDisplayType>(mir_connection_get_egl_native_display(m_connection)));

	EGLint major = 0, minor = 0;
	if (eglInitialize(m_eglDisplay, &major, &minor) != EGL_TRUE)
		throw EGLError("Can't initialize EGL!");

	std::cout << "Initalized EGL " << major << "." << minor << std::endl;
}

EGLConfig MirNativeWindow::getEglConfig()
{
	const EGLint attribList[] =
	{
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 0,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_NONE
	};
	EGLConfig eglConfig;
	EGLint numConfigs = 0;
	eglChooseConfig(m_eglDisplay, attribList, &eglConfig, 1, &numConfigs);
	if (numConfigs != 1)
		throw EGLError("No suitable EGL config found!");

	return eglConfig;
}

void MirNativeWindow::createFullScreenSurface(const char* surfaceName, int outputId, MirPixelFormat pixelFormat)
{
	// it seems the size is ignored when the surface is fullscreen
	std::unique_ptr<MirSurfaceSpec, decltype(&mir_surface_spec_release)>
			spec(mir_connection_create_spec_for_normal_surface(m_connection, 0, 0, pixelFormat), mir_surface_spec_release);
	if (!spec)
		throw MirError("Can't create Mit surface specification!");

	mir_surface_spec_set_name(spec.get(), surfaceName);
	mir_surface_spec_set_fullscreen_on_output(spec.get(), outputId);

	m_surface.reset(mir_surface_create_sync(spec.get()));

	if (!mir_surface_is_valid(m_surface.get()))
		throw MirSurfaceError(m_surface.get());

	MirSurfaceParameters params;
	mir_surface_get_parameters(m_surface.get(), &params);
	m_width = params.width;
	m_height = params.height;

	{
		std::unique_ptr<MirCursorConfiguration, decltype(&mir_cursor_configuration_destroy)>
				cursorConfig(mir_cursor_configuration_from_name(mir_default_cursor_name), mir_cursor_configuration_destroy);
		mir_surface_configure_cursor(m_surface.get(), cursorConfig.get());
	}

	mir_surface_set_event_handler(m_surface.get(), MirNativeWindow_SurfaceEventHandlerTrampoline, this);
}

void MirNativeWindow_SurfaceEventHandlerTrampoline(MirSurface*, MirEvent const* event, void* context) noexcept
{
	MirNativeWindow *self = static_cast<MirNativeWindow*>(context);

	// don't propagate exceptions into the C code
	try
	{
		self->surfaceEventHandler(event);
	}
	catch (const std::runtime_error& e)
	{
		std::cerr << "MirNativeWindow_SurfaceEventHandlerTrampoline: caught exception: " << e.what() << std::endl;
	}

	catch (...)
	{
		std::cerr << "MirNativeWindow_SurfaceEventHandlerTrampoline: caught unknown exception" << std::endl;
	}
}
