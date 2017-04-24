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
#ifndef DEMO_RENDERER_H
#define DEMO_RENDERER_H

#include <chrono>

#include <mir_toolkit/events/event.h>
#include <GLES2/gl2.h>

#include <glm/glm.hpp>

#include "MirNativeWindowRenderer.h"
#include "MirNativeWindowControl.h"
#include "SwipeGesture.h"

class DemoRenderer: public MirNativeWindowRenderer, private SwipeGesture::Listener
{
public:
	DemoRenderer();
	virtual void run(MirNativeWindowControl& nativeWindow) override;
	virtual void handleEvent(const MirEvent* event) override;

private:
	typedef std::chrono::steady_clock clock;

	void renderFrame();
	void handleInputEvent(const MirInputEvent* inputEvent);
	void handleInputTouchEvent(const MirTouchEvent* touchEvent);
	void handleKeyboardEvent(const MirKeyboardEvent* keyboardEvent);
	void handlePointerEvent(const MirPointerEvent* pointerEvent);

	void onPointerDown(float x, float y);
	void onPointerMove(float x, float y);
	void onPointerUp(float x, float y);

	virtual void onSwipe(float x, float y) override;

	void rotateCube(float x, float y);

	static void rotateAlongAxis(float& rotationAngle, float &rotationAngularSpeed, float secsSinceLastFrame);

	enum class PointerState
	{
		Up,
		PointerDown,
		FingerDown
	};

	GLuint m_mvpMatrixIndex;
	glm::mat4 m_projectionMatrix;
	GLuint m_cubeVertexCount;
	PointerState m_pointerState;
	MirTouchId m_fingerId;

	float m_lastX;
	float m_lastY;

	float m_rotationAngularSpeedX;
	float m_rotationAngularSpeedY;

	float m_rotationAngleX;
	float m_rotationAngleY;

	bool m_lastFrameTimeStampValid;
	clock::time_point m_lastFrameTimeStamp;
	SwipeGesture m_swipeGesture;
};

#endif // DEMO_RENDERER_H
