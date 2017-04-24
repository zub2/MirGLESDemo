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
#ifndef SWIPE_GESTURE_H
#define SWIPE_GESTURE_H

#include <chrono>

class SwipeGesture
{
public:
	struct Listener
	{
		virtual void onSwipe(float dx, float dy) = 0;
	};

	SwipeGesture(Listener & listener):
		m_listener(listener)
	{}

	void down(float x, float y);
	void up(float x, float y);

private:
	typedef std::chrono::steady_clock clock;

	Listener& m_listener;
	bool m_isDown;
	clock::time_point m_timeStamp;
	float m_x;
	float m_y;

	static const clock::duration SWIPE_DURATION;
	static const float SWIPE_MIN_LENGTH_SQUARE;
};

#endif // SWIPE_GESTURE_H
