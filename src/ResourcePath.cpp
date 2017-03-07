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
#include "ResourcePath.h"

#include <stdexcept>
#include <memory>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <limits.h>

static const char MEDIA_PREFIX[] = "media/";

std::string getResourcePath(const std::string& resource)
{
	std::string result;

	// assume the working directory contains the media inside the media directory
	result.reserve(sizeof(MEDIA_PREFIX) -1 /* for the 0 terminating byte */+ resource.size());
	result = MEDIA_PREFIX;
	result += resource;
	return result;
}
