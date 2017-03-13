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
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <mir_toolkit/mir_client_library.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

inline std::string getMirErrorString(MirConnection* connection)
{
	return mir_connection_get_error_message(connection);
}

inline std::string getMirErrorString(MirSurface* surface)
{
	return mir_surface_get_error_message(surface);
}

inline std::string getEGLErrorString()
{
	const EGLint error = eglGetError();
	switch(error)
	{
	case EGL_NOT_INITIALIZED:
		return "EGL_NOT_INITIALIZED";
	case EGL_BAD_ACCESS:
		return "EGL_BAD_ACCESS";
	case EGL_BAD_ALLOC:
		return "EGL_BAD_ALLOC";
	case EGL_BAD_ATTRIBUTE:
		return "EGL_BAD_ATTRIBUTE";
	case EGL_BAD_CONTEXT:
		return "EGL_BAD_CONTEXT";
	case EGL_BAD_CONFIG:
		return "EGL_BAD_CONFIG";
	case EGL_BAD_CURRENT_SURFACE:
		return "EGL_BAD_CURRENT_SURFACE";
	case EGL_BAD_DISPLAY:
		return "EGL_BAD_DISPLAY";
	case EGL_BAD_SURFACE:
		return "EGL_BAD_SURFACE";
	case EGL_BAD_MATCH:
		return "EGL_BAD_MATCH";
	case EGL_BAD_PARAMETER:
		return "EGL_BAD_PARAMETER";
	case EGL_BAD_NATIVE_PIXMAP:
		return "EGL_BAD_NATIVE_PIXMAP";
	case EGL_BAD_NATIVE_WINDOW:
		return "EGL_BAD_NATIVE_WINDOW";
	case EGL_CONTEXT_LOST:
		return "EGL_CONTEXT_LOST";
	default:
		return ""; // no info available
	}
}

inline std::string getGLErrorString()
{
	const EGLint error = eglGetError();
	switch (error)
	{
	case GL_INVALID_ENUM:
		return "GL_INVALID_ENUM";
	case GL_INVALID_VALUE:
		return "GL_INVALID_VALUE";
	case GL_INVALID_OPERATION:
		return "GL_INVALID_OPERATION";
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		return "GL_INVALID_FRAMEBUFFER_OPERATION";
	case GL_OUT_OF_MEMORY:
		return "GL_OUT_OF_MEMORY";
	default:
		return ""; // no info available
	}
}

inline std::string formatErrorString(std::string&& description, const std::string& detail)
{
	std::string r = std::move(description);

	if (!detail.empty())
	{
		r += " (";
		r += detail;
		r += ")";
	}

	return r;
}

class MirError: public std::runtime_error
{
	using std::runtime_error::runtime_error;
};

class MirConnectionError: public MirError
{
public:
	MirConnectionError(MirConnection* connection): MirError(getMirErrorString(connection))
	{}
};

class MirSurfaceError: public MirError
{
public:
	MirSurfaceError(MirSurface* surface): MirError(getMirErrorString(surface))
	{}
};

class EGLError: public std::runtime_error
{
public:
	EGLError(std::string description): std::runtime_error(formatErrorString(std::move(description), getEGLErrorString()))
	{}
};

class GLError: public std::runtime_error
{
public:
	GLError(std::string description): std::runtime_error(formatErrorString(std::move(description), getGLErrorString()))
	{}
};

#endif // EXCEPTIONS_H
