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
#ifndef MIR_NATIVE_WINDOW_RENDERER_H
#define MIR_NATIVE_WINDOW_RENDERER_H

#include <mir_toolkit/events/event.h>

#include "MirNativeWindowControl.h"

class MirNativeWindowRenderer
{
public:
	virtual void run(MirNativeWindowControl& nativeWindow) = 0;
	virtual void handleEvent(const MirEvent* event) = 0;

	virtual ~MirNativeWindowRenderer()
	{}
};

#endif // MIR_NATIVE_WINDOW_RENDERER_H
