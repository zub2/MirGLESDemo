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
#ifndef MIR_NATIVE_WINDOW_H
#define MIR_NATIVE_WINDOW_H

#include <mir_toolkit/mir_client_library.h>
#include <mir_toolkit/events/input/input_event.h>

#include <EGL/egl.h>

#include <memory>
#include <mutex>

#include "MirNativeWindowControl.h"
#include "MirNativeWindowRenderer.h"

extern "C" void MirNativeWindow_SurfaceEventHandlerTrampoline(MirSurface*, MirEvent const* event, void* context) noexcept;

class MirNativeWindow: private MirNativeWindowControl
{
public:
	MirNativeWindow(MirConnection* connection, int outputId, const char* surfaceName, std::shared_ptr<MirNativeWindowRenderer> renderer);
	~MirNativeWindow();

	void run();

private:
	virtual int getWidth() override;
	virtual int getHeight() override;
	virtual void swapBuffers() override;

	void surfaceEventHandler(const MirEvent* event);

	void initEGL();
	EGLConfig getEglConfig();
	void createFullScreenSurface(const char* surfaceName, int outputId, MirPixelFormat pixelFormat);

	// let the trampoline reach surfaceEventHandler
	friend void MirNativeWindow_SurfaceEventHandlerTrampoline(MirSurface*, MirEvent const* event, void* context) noexcept;

	MirConnection *m_connection;
	std::unique_ptr<MirSurface, decltype(&mir_surface_release_sync)> m_surface;

	int m_width;
	int m_height;
	EGLDisplay m_eglDisplay;
	EGLSurface m_eglSurface;

	std::mutex m_eventMutex;

	std::shared_ptr<MirNativeWindowRenderer> m_renderer;
};

#endif // MIR_NATIVE_WINDOW_H
