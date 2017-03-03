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
#include "PNGLoader.h"
#include "Image.h"

#include <memory>

#include <png.h>
#include <cstdio>
#include <csetjmp>
#include <cstring>
#include <iostream>

/**
 * Read any color_type into 8bit depth, RGBA format.
 * Taken from https://gist.github.com/niw/5963798.
 */
static void setPngReadOptions(png_structp pngReader, png_infop pngInfo)
{
	const png_byte colorType = png_get_color_type(pngReader, pngInfo);
	const png_byte bitDepth  = png_get_bit_depth(pngReader, pngInfo);

	if (bitDepth == 16)
		png_set_strip_16(pngReader);

	if (colorType == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(pngReader);

	// PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
	if (colorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8)
		png_set_expand_gray_1_2_4_to_8(pngReader);

	if (png_get_valid(pngReader, pngInfo, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(pngReader);

	// These color_type don't have an alpha channel then fill it with 0xff.
	if (colorType == PNG_COLOR_TYPE_RGB || colorType == PNG_COLOR_TYPE_GRAY || colorType == PNG_COLOR_TYPE_PALETTE)
		png_set_filler(pngReader, 0xff, PNG_FILLER_AFTER);

	if (colorType == PNG_COLOR_TYPE_GRAY || colorType == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(pngReader);

	png_read_update_info(pngReader, pngInfo);
}

// inspired by
// https://blog.nobel-joergensen.com/2010/11/07/loading-a-png-as-texture-in-opengl-using-libpng/
// http://www.libpng.org/pub/png/book/chapter13.html
// https://gist.github.com/niw/5963798
Image loadPNG(const std::string& fileName)
{
	std::cout << "Loading file: " << fileName.c_str() << std::endl;

	std::unique_ptr<FILE,decltype(&fclose)> file(std::fopen(fileName.c_str(), "rb"), fclose);
	if (!file)
		throw std::runtime_error(std::string("Can't open file ") + fileName);

	png_infop pngInfo = nullptr;
	auto pngDeleter = [&pngInfo](png_structp pngReader){
		png_destroy_read_struct(&pngReader, &pngInfo, nullptr);
	};
	std::unique_ptr<png_struct, decltype(pngDeleter)> pngReader(nullptr, pngDeleter);

	pngReader.reset(png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr));
	if (!pngReader)
		throw std::runtime_error("Can't create PNG reader!");

	pngInfo = png_create_info_struct(pngReader.get());
	if (!pngInfo)
		throw std::runtime_error("Can't create PNG reader info structure!");

	if (setjmp(png_jmpbuf(pngReader.get())))
		throw std::runtime_error("Can't decode the file.");

	png_init_io(pngReader.get(), file.get());
	png_set_sig_bytes(pngReader.get(), 0);
	png_read_info(pngReader.get(), pngInfo);

	const png_uint_32 width = png_get_image_width(pngReader.get(), pngInfo);
	const png_uint_32 height = png_get_image_height(pngReader.get(), pngInfo);
	std::cout << "loadPNG: width = " << width << ", height = " << height << std::endl;

	setPngReadOptions(pngReader.get(), pngInfo);

	const size_t rowSize = png_get_rowbytes(pngReader.get(), pngInfo);

	// buffer holding the whole image
	std::unique_ptr<unsigned char[]> data(new unsigned char[rowSize * height]);

	/*
	 * Allocate and fill an array of row pointers: one pointer for each row.
	 * The rows in a PNG are ordered top to bottom but OpenGL expects the rows bottom to top.
	 * So the rowPointers vectors is filled reversed: first row pointer points to the last
	 * row in the output buffer etc.
	 */
	std::unique_ptr<png_bytep[]> rowPointers(new png_bytep[height]);
	for (size_t i = 0; i < height; i++)
		rowPointers[i] = data.get() + (height - 1 - i) * rowSize;

	png_read_image(pngReader.get(), rowPointers.get());

	return Image(width, height, std::move(data));
}
