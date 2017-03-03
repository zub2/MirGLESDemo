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
#include "Image.h"

Image::Image(unsigned width, unsigned height, std::unique_ptr<unsigned char[]> data):
	m_width(width),
	m_height(height),
	m_data(std::move(data))
{}

unsigned Image::getWidth() const
{
	return m_width;
}

unsigned Image::getHeight() const
{
	return m_height;
}

unsigned char* Image::getData() const
{
	return m_data.get();
}
