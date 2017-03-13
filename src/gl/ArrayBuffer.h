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
#ifndef GL_ARRAY_BUFFER_H
#define GL_ARRAY_BUFFER_H

#include <memory>
#include <GLES2/gl2.h>

class ArrayBuffer
{
public:
	ArrayBuffer();
	~ArrayBuffer();

	void bind();

	GLuint getGLBuffer() const
	{
		return m_buffer;
	}

	// allow move, disallow copy
	ArrayBuffer& operator=(ArrayBuffer&&) = default;
	ArrayBuffer(ArrayBuffer&&) = default;
	ArrayBuffer& operator=(const ArrayBuffer&) = delete;
	ArrayBuffer(const ArrayBuffer&) = delete;

private:
	GLuint m_buffer;
};

#endif // GL_ARRAY_BUFFER_H
