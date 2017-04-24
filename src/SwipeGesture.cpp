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

#include "SwipeGesture.h"

#include <iostream>

const SwipeGesture::clock::duration SwipeGesture::SWIPE_DURATION = std::chrono::milliseconds(200);
const float SwipeGesture::SWIPE_MIN_LENGTH_SQUARE = 300.0f;

void SwipeGesture::down(float x, float y)
{
	if (!m_isDown)
	{
		m_isDown = true;
		m_timeStamp = clock::now();
		m_x = x;
		m_y = y;
	}
}

void SwipeGesture::up(float x, float y)
{
	if (m_isDown)
	{
		const clock::duration dt = clock::now() - m_timeStamp;
		if (dt < SWIPE_DURATION)
		{
			const float dx = x - m_x;
			const float dy = y - m_y;

			if (dx*dx + dy*dy >= SWIPE_MIN_LENGTH_SQUARE)
			{
				const float swipeCoeff = static_cast<float>(SWIPE_DURATION.count()) / static_cast<float>(dt.count());
				std::cout << "SwipeGesture::up: dx=" << dx << ", dy=" << dy << ", swipeCoeff=" << swipeCoeff << std::endl;
				m_listener.onSwipe(dx * swipeCoeff, dy * swipeCoeff);
			}
		}
		m_isDown = false;
	}
}
