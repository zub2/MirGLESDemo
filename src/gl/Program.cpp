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
#include "Program.h"
#include "../Exceptions.h"

#include <stdexcept>
#include <memory>

Program::Program(const Shader& vertexShader, const Shader& fragmentShader):
	m_isLinked(false)
{
	m_program = glCreateProgram();
	if (m_program == 0)
		throw std::runtime_error("Can't create a new program.");

	// no need to keep the shaders, OpenGL keeps them alive as long as the program lives
	glAttachShader(m_program, vertexShader.getGLShader());
	glAttachShader(m_program, fragmentShader.getGLShader());
}

void Program::link()
{
	glLinkProgram(m_program);

	GLint isLinked = 0;
	glGetProgramiv(m_program, GL_LINK_STATUS, &isLinked);
	if (isLinked == 0)
	{
		GLint infoLogLen = 0;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &infoLogLen);
		if (infoLogLen > 0)
		{
			std::unique_ptr<char[]> infoLog(new char[infoLogLen]);
			glGetProgramInfoLog(m_program, infoLogLen, nullptr, infoLog.get());
			throw std::runtime_error(std::string("Can't link program: ") + infoLog.get());
		}
		throw std::runtime_error("Can't link program: no error info available");
	}

	m_isLinked = true;
}

GLuint Program::getAttribute(const char* name)
{
	if (!m_isLinked)
		throw std::runtime_error("getAttribute() can only be called after the program has been linked");

	GLint index = glGetAttribLocation(m_program, name);
	if (index < 0)
	{
		throw GLError(std::string("Can't get location of attribute '") + name + "'");
	}

	return static_cast<GLuint>(index);
}

GLuint Program::getUniform(const char* name)
{
	if (!m_isLinked)
		throw std::runtime_error("getUniform() can only be called after the program has been linked");

	GLint index = glGetUniformLocation(m_program, name);
	if (index < 0)
	{
		throw GLError(std::string("Can't get location of uniform '") + name + "'");
	}

	return static_cast<GLuint>(index);
}

Program::~Program()
{
	glDeleteProgram(m_program);
}
