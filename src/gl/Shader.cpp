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
#include "Shader.h"
#include <stdexcept>
#include <memory>

static GLenum getGLShaderType(ShaderType type)
{
	switch (type)
	{
	case ShaderType::Vertex:
		return GL_VERTEX_SHADER;
	case ShaderType::Fragment:
		return GL_FRAGMENT_SHADER;
	}

	throw std::runtime_error("getGLShaderType: unexpected shader type " + std::to_string(static_cast<int>(type)));
}

Shader::Shader(ShaderType type, const char* program)
{
	m_shader = glCreateShader(getGLShaderType(type));
	if (m_shader == 0)
		throw std::runtime_error("Can't create a new shader.");

	glShaderSource(m_shader, 1, &program, nullptr);
	glCompileShader(m_shader);

	GLint isCompiled = 0;
	glGetShaderiv(m_shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == 0)
	{
		GLint infoLogLen = 0;
		glGetShaderiv(m_shader, GL_INFO_LOG_LENGTH, &infoLogLen);
		if (infoLogLen > 0)
		{
			std::unique_ptr<char[]> infoLog(new char[infoLogLen]);
			glGetShaderInfoLog(m_shader, infoLogLen, nullptr, infoLog.get());
			throw std::runtime_error(std::string("Can't compile shader: ") + infoLog.get());
		}
		throw std::runtime_error("Can't compile shader: no error info available");
	}
}

Shader::~Shader()
{
	glDeleteShader(m_shader);
}
