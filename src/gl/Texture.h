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
#ifndef GL_TEXTURE_H
#define GL_TEXTURE_H

#include "../Image.h"
#include <GLES2/gl2.h>

class Texture2D
{
public:
	explicit Texture2D(const Image &image);
	~Texture2D();

	GLuint getGLTexture() const
	{
		return m_texture;
	}

	// allow move, disallow copy
	Texture2D& operator=(Texture2D&&) = default;
	Texture2D(Texture2D&&) = default;
	Texture2D& operator=(const Texture2D&) = delete;
	Texture2D(const Texture2D&) = delete;

private:
	GLuint m_texture;
};

#endif // GL_TEXTURE_H
