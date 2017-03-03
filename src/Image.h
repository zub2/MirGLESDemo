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
#ifndef IMAGE_H
#define IMAGE_H

#include <memory>

class Image
{
public:
	Image(unsigned width, unsigned height, std::unique_ptr<unsigned char[]> data);
	unsigned getWidth() const;
	unsigned getHeight() const;
	unsigned char* getData() const;
	// TODO: format

private:
	unsigned m_width;
	unsigned m_height;
	std::unique_ptr<unsigned char[]> m_data;
};

#endif // IMAGE_H
