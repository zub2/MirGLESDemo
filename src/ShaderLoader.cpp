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
#include "ShaderLoader.h"
#include "gl/Shader.h"
#include "ResourcePath.h"

#include <stdexcept>
#include <fstream>
#include <memory>
#include <iostream>

std::shared_ptr<Shader> loadShader(ShaderType type, const std::string& fileName)
{
	std::ifstream shaderFile(fileName);
	if (!shaderFile.good())
		throw std::runtime_error(std::string("Can't open shader file '") + fileName + "'");

	shaderFile.seekg(0, shaderFile.end);
	const size_t shaderSize = shaderFile.tellg();
	shaderFile.seekg(0, shaderFile.beg);

	std::unique_ptr<char[]> shaderProgram(new char[shaderSize + 1]);
	shaderFile.read(shaderProgram.get(), shaderSize);
	shaderProgram[shaderSize] = 0;

	return std::make_shared<Shader>(type, shaderProgram.get());
}
