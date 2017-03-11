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
#ifndef GL_PROGRAM_H
#define GL_PROGRAM_H

#include "Shader.h"

#include <GLES2/gl2.h>

class Program
{
public:
	Program(const Shader& vertexShader, const Shader& fragmentShader);
	~Program();

	void bindAttribute(GLuint index, const char *name);
	void link();

	GLuint getGLProgram() const
	{
		return m_program;
	}

	// allow move, disallow copy
	Program& operator=(Program&&) = default;
	Program(Program&&) = default;
	Program& operator=(const Program&) = delete;
	Program(const Program&) = delete;

private:
	GLuint m_program;
};

#endif // GL_PROGRAM_H
