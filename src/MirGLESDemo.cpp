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
#include <iostream>
#include <stdexcept>

#include <cstring>

#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>

#include <mir_toolkit/mir_client_library.h>
#include <mir_toolkit/events/input/input_event.h>

#include <GLES2/gl2.h>
#include <EGL/egl.h>

MirConnection *connection = nullptr;
MirSurface *surface = nullptr;
boost::mutex eventMutex;
bool fingerDown = false;

static void event_handler(MirSurface*, MirEvent const* event, void*)
{
	boost::lock_guard<boost::mutex> guard(eventMutex);
	(void)guard;

	MirEventType type = mir_event_get_type(event);
	if (type == mir_event_type_input)
	{
		std::cout << "got mir_event_type_input" << std::endl;
		MirInputEvent const* inputEvent = mir_event_get_input_event(event);
		MirInputEventType inputType = mir_input_event_get_type(inputEvent);
		switch (inputType) {
		case mir_input_event_type_key:
			std::cout << "input type KEY" << std::endl;
			break;
		case mir_input_event_type_touch:
			{
				std::cout << "input type TOUCH" << std::endl;
				MirTouchEvent const* touchEvent = mir_input_event_get_touch_event(inputEvent);
				MirTouchAction action = mir_touch_event_action(touchEvent, 0);
				float touchX = mir_touch_event_axis_value(touchEvent, 0, mir_touch_axis_x);
				float touchY = mir_touch_event_axis_value(touchEvent, 0, mir_touch_axis_y);
				const char *actionStr;
				switch (action)
				{
				case mir_touch_action_up:
					actionStr = "up";
					break;
				case mir_touch_action_down:
					actionStr = "down";
					break;
				case mir_touch_action_change:
					actionStr = "change";
					break;
				}

				std::cout << "input touch finger#0 " << actionStr << " at (" << touchX << "," << touchY << ")" << std::endl;

				if (action == mir_touch_action_down)
					fingerDown = true;
				else if (action == mir_touch_action_up)
					fingerDown = false;
			}
			break;
		case mir_input_event_type_pointer:
			std::cout << "input type POINTER" << std::endl;
			break;
		}
	}
}

const char* pixelFormatToString(MirPixelFormat f)
{
	switch (f)
	{
	case mir_pixel_format_abgr_8888:
		return "mir_pixel_format_abgr_8888";
	case mir_pixel_format_xbgr_8888:
		return "mir_pixel_format_xbgr_8888";
	case mir_pixel_format_argb_8888:
		return "mir_pixel_format_argb_8888";
	case mir_pixel_format_xrgb_8888:
		return "mir_pixel_format_xrgb_8888";
	case mir_pixel_format_bgr_888:
		return "mir_pixel_format_bgr_888";
	case mir_pixel_format_rgb_888:
		return "mir_pixel_format_rgb_888";
	case mir_pixel_format_rgb_565:
		return "mir_pixel_format_rgb_565";
	case mir_pixel_format_rgba_5551:
		return "mir_pixel_format_rgba_5551";
	case mir_pixel_format_rgba_4444:
		return "mir_pixel_format_rgba_4444";
	default:
		return "unknown";
	}
}

MirPixelFormat pickFormat(MirConnection* connection)
{
	MirPixelFormat availableFormats[mir_pixel_formats];
	unsigned int formatCount;

	mir_connection_get_available_surface_formats(connection, availableFormats,
			sizeof(availableFormats)/sizeof(availableFormats[0]), &formatCount);

	// dump all formats
	for (int i = 0; i < formatCount; i++)
	{
		const char* s = pixelFormatToString(availableFormats[i]);
		std::cout << "available format[" << i << "]: " << s << std::endl;
	}

	MirPixelFormat format = mir_pixel_format_invalid;
	int formatWeight = 0;
	for (int i = 0; i < formatCount; i++)
	{
		switch (availableFormats[i])
		{
		case mir_pixel_format_abgr_8888:
			if (formatWeight < 11)
			{
				formatWeight = 11;
				format = mir_pixel_format_abgr_8888;
			}
			break;
		case mir_pixel_format_argb_8888:
			if (formatWeight < 8)
			{
				formatWeight = 8;
				format = mir_pixel_format_argb_8888;
			}
			break;
		case mir_pixel_format_bgr_888:
			if (formatWeight < 9)
			{
				formatWeight = 9;
				format = mir_pixel_format_bgr_888;
			}
			break;
		case mir_pixel_format_rgb_888:
			if (formatWeight < 10)
			{
				formatWeight = 10;
				format = mir_pixel_format_rgb_888;
			}
			break;
		}
	}

	std::cout << "using format: " << pixelFormatToString(format) << std::endl;

	return format;
}

struct OutputConfig
{
	OutputConfig(int id, int w, int h):
		outputId(id),
		width(w),
		height(h)
	{}

	int outputId;
	int width;
	int height;
};

const char* getMirOutputTypeString(MirOutputType mirOutputType)
{
	switch (mirOutputType)
	{
	case mir_output_type_vga:
		return "mir_output_type_vga";
	case mir_output_type_dvii:
		return "mir_output_type_dvii";
	case mir_output_type_dvid:
		return "mir_output_type_dvid";
	case mir_output_type_dvia:
		return "mir_output_type_dvia";
	case mir_output_type_composite:
		return "mir_output_type_composite";
	case mir_output_type_svideo:
		return "mir_output_type_svideo";
	case mir_output_type_lvds:
		return "mir_output_type_lvds";
	case mir_output_type_component:
		return "mir_output_type_component";
	case mir_output_type_ninepindin:
		return "mir_output_type_ninepindin";
	case mir_output_type_displayport:
		return "mir_output_type_displayport";
	case mir_output_type_hdmia:
		return "mir_output_type_hdmia";
	case mir_output_type_hdmib:
		return "mir_output_type_hdmib";
	case mir_output_type_tv:
		return "mir_output_type_tv";
	case mir_output_type_edp:
		return "mir_output_type_edp";
//    case mir_output_type_virtual:
//        return "mir_output_type_virtual";
	case mir_output_type_unknown:
		return "mir_output_type_unknown";
	default:
		return "<<unknown>>";
	}
}

OutputConfig getOutputConfig(MirConnection* connection)
{
	bool haveSize = false;
	int width;
	int height;
	int displayId;
	MirOutputType outputType;

	MirDisplayConfig *displayConfig = mir_connection_create_display_configuration(connection);
	if (displayConfig)
	{
		const int numOutputs = mir_display_config_get_num_outputs(displayConfig);

		// find the first connected (or, not-disconnected) && enabled output
		int i = 0;
		while (!haveSize && i < numOutputs)
		{
			MirOutput const* mirOutput = mir_display_config_get_output(displayConfig, i);
			if (mir_output_get_connection_state(mirOutput) != mir_output_connection_state_disconnected
					&& mir_output_is_enabled(mirOutput))
			{
				MirOutputMode const* outputMode = mir_output_get_current_mode(mirOutput);
				width = mir_output_mode_get_width(outputMode);
				height = mir_output_mode_get_height(outputMode);
				displayId = mir_output_get_id(mirOutput);
				outputType = mir_output_get_type(mirOutput);
				haveSize = true;
			}
			i++;
		}
	}
	mir_display_config_release(displayConfig);

	if (!haveSize)
		throw std::runtime_error("Can't determine output #0 size!");

	std::cout << "Using display #" << displayId << " (" << getMirOutputTypeString(outputType) << ")" << std::endl;

	return OutputConfig(displayId, width, height);
}

EGLConfig getEglConfig(EGLDisplay eglDisplay)
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
	eglChooseConfig(eglDisplay, attribList, &eglConfig, 1, &numConfigs);
	if (numConfigs != 1)
		throw std::runtime_error("No suitable EGL config found!");

	return eglConfig;
}

int main()
{
	std::cout << "Connecting to mir..." << std::endl;
	connection = mir_connect_sync(nullptr /*default*/, "PlainProjectTest");
	std::cout << "mir_connect returned" << std::endl;

	if (connection == nullptr || !mir_connection_is_valid(connection))
	{
		const char *error = "Unknown error";
		if (connection != nullptr)
			error = mir_connection_get_error_message(connection);
		std::cerr << "Can't connect to server: " << error << std::endl;
		return 1;
	}

	OutputConfig outputConfig = getOutputConfig(connection);
	std::cout << "Detected output #" << outputConfig.outputId << " size: " << outputConfig.width << "x" << outputConfig.height << std::endl;

	EGLDisplay eglDisplay = eglGetDisplay(reinterpret_cast<EGLNativeDisplayType>(mir_connection_get_egl_native_display(connection)));
	EGLint major = 0, minor = 0;
	EGLBoolean success = eglInitialize(eglDisplay, &major, &minor);
	if (success != EGL_TRUE)
		throw std::runtime_error("Can't initialize EGL!");
	std::cout << "Initalized EGL " << major << "." << minor << std::endl;

	EGLConfig eglConfig = getEglConfig(eglDisplay);
	MirPixelFormat pixelFormat = mir_connection_get_egl_pixel_format(connection, eglDisplay, eglConfig);

	MirSurfaceSpec *spec = mir_connection_create_spec_for_normal_surface(connection, outputConfig.width, outputConfig.height, pixelFormat);
	mir_surface_spec_set_name(spec, "MirGLESDemo Surface");

	mir_surface_spec_set_fullscreen_on_output(spec, outputConfig.outputId);

	surface = mir_surface_create_sync(spec);
	std::cout << "Surface created" << std::endl;

	mir_surface_spec_release(spec);

	if (!mir_surface_is_valid(surface))
	{
		const char *error = mir_surface_get_error_message(surface);
		std::cerr << "Can't create surface: " << error << std::endl;
		return 1;
	}

	mir_surface_set_event_handler(surface, event_handler, nullptr);

	MirCursorConfiguration* cursorConfig = mir_cursor_configuration_from_name(mir_default_cursor_name);
	mir_surface_configure_cursor(surface, cursorConfig);
	mir_cursor_configuration_destroy(cursorConfig);

	MirBufferStream *bufferStream = mir_surface_get_buffer_stream(surface);

	// EGLNativeWindowType is typedef's to (X) Window... blah
	EGLNativeWindowType eglNativeWindow = reinterpret_cast<EGLNativeWindowType>(mir_buffer_stream_get_egl_native_window(bufferStream));

	EGLSurface eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, eglNativeWindow, nullptr);
	if (eglSurface == EGL_NO_SURFACE)
		throw std::runtime_error("Can't create EGL surface!");

	EGLint contextAttribList[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};
	EGLContext eglContext = eglCreateContext(eglDisplay, eglConfig, EGL_NO_CONTEXT, contextAttribList);
	if (eglContext == EGL_NO_CONTEXT)
		throw std::runtime_error("Can't create EGL context!");

	EGLBoolean ok = eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
	if (ok != EGL_TRUE)
		throw std::runtime_error("Can't make context current!");

	eglSwapInterval(eglDisplay, 1);

	glClearColor(0., 0., 1., 1.);
	glViewport(0, 0, outputConfig.width, outputConfig.height);

	// We can keep exchanging the current buffer for a new one
	bool pressed = false;
	for (int i = 0; i < 1000; i++)
	{
		{
			boost::lock_guard<boost::mutex> guard(eventMutex);
			(void)guard;
			pressed = fingerDown;
		}

		if (pressed)
			glClearColor(1., 0., 0., 1.);
		else
			glClearColor(0., 0., 1., 1.);

		glClear(GL_COLOR_BUFFER_BIT);
		eglSwapBuffers(eglDisplay, eglSurface);
	}

	eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglTerminate(eglDisplay);

	mir_surface_release_sync(surface);
	std::cout << "Surface released" << std::endl;

	mir_connection_release(connection);
	std::cout << "Connection released" << std::endl;

	return 0;
}
