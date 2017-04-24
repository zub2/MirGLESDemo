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
#include "DemoRenderer.h"
#include "ResourcePath.h"
#include "PNGLoader.h"
#include "ShaderLoader.h"

#include "gl/Program.h"
#include "gl/ArrayBuffer.h"
#include "gl/Texture.h"

#include <iostream>
#include <stdexcept>
#include <cmath>
#include <vector>
#include <thread>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

DemoRenderer::DemoRenderer():
	m_mvpMatrixIndex(0),
	m_cubeVertexCount(0),
	m_pointerState(PointerState::Up),
	m_fingerId(0),
	m_lastX(0),
	m_lastY(0),
	m_rotationAngularSpeedX(0.0f),
	m_rotationAngularSpeedY(0.0f),
	m_rotationAngleX(0.0f),
	m_rotationAngleY(0.0f),
	m_lastFrameTimeStampValid(false),
	m_swipeGesture(*this)
{}

namespace
{
	template<typename VEC>
	void addFace(std::vector<VEC>& triangles, const VEC& a, const VEC& b, const VEC& c, const VEC& d)
	{
		// 1st triangle
		triangles.push_back(a);
		triangles.push_back(b);
		triangles.push_back(d);

		// 2nd triangle
		triangles.push_back(b);
		triangles.push_back(d);
		triangles.push_back(c);
	}

	void clampAngle(float& angle)
	{
		while (angle > M_PI)
			angle -= 2.0 * M_PI;
		while (angle <= -M_PI)
			angle += 2.0 * M_PI;
	}
}

void DemoRenderer::run(MirNativeWindowControl& nativeWindow)
{
	std::cout << "Loading shader" << std::endl;
	std::shared_ptr<Shader> vertexShader = loadShader(ShaderType::Vertex, getResourcePath("vertex_shader.glslv"));
	std::shared_ptr<Shader> fragmentShader = loadShader(ShaderType::Fragment, getResourcePath("fragment_shader.glslf"));

	Program program(*vertexShader, *fragmentShader);
	program.link();
	glUseProgram(program.getGLProgram());

	glViewport(0, 0, nativeWindow.getWidth(), nativeWindow.getHeight());
	glClearColor(0.2, 0.4, 0., 1.);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	const GLint vertexIndex = program.getAttribute("vPosition");
	const GLint colorIndex = program.getAttribute("vTexCoord");
	m_mvpMatrixIndex = program.getUniform("MVPMatrix");
	const GLint textureSamplerIndex = program.getUniform("textureSampler");

	std::vector<glm::vec3> v; // vertices
	std::vector<glm::vec2> c; // text coords
	v.reserve(6 /* faces */ * 2 /* 2 triangles for each face */);
	c.reserve(v.capacity());

	constexpr float HALF = 1.0/2.0;
	constexpr float THIRD = 1.0/3.0;

	// z = +1, value 1
	addFace(v, glm::vec3(-1, -1, +1), glm::vec3(-1, +1, +1), glm::vec3(+1, +1, +1), glm::vec3(+1, -1, +1));
	addFace(c, glm::vec2(0, HALF), glm::vec2(0, 1), glm::vec2(THIRD, 1), glm::vec2(THIRD, HALF));

	// z = -1, value 6
	addFace(v, glm::vec3(+1, -1, -1), glm::vec3(+1, +1, -1), glm::vec3(-1, +1, -1), glm::vec3(-1, -1, -1));
	addFace(c, glm::vec2(2*THIRD, 0), glm::vec2(2*THIRD, HALF), glm::vec2(1, HALF), glm::vec2(1, 0));

	// x = +1, value 2
	addFace(v, glm::vec3(+1, -1, +1), glm::vec3(+1, +1, +1), glm::vec3(+1, +1, -1), glm::vec3(+1, -1, -1));
	addFace(c, glm::vec2(THIRD, HALF), glm::vec2(THIRD, 1), glm::vec2(2*THIRD, 1), glm::vec2(2*THIRD, HALF));

	// x = -1, value 5
	addFace(v, glm::vec3(-1, -1, -1), glm::vec3(-1, +1, -1), glm::vec3(-1, +1, +1), glm::vec3(-1, -1, +1));
	addFace(c, glm::vec2(THIRD, 0), glm::vec2(THIRD, HALF), glm::vec2(2*THIRD, HALF), glm::vec2(2*THIRD, 0));

	// y = +1, value 3
	addFace(v, glm::vec3(-1, +1, +1), glm::vec3(-1, +1, -1), glm::vec3(+1, +1, -1), glm::vec3(+1, +1, +1));
	addFace(c, glm::vec2(2*THIRD, HALF), glm::vec2(2*THIRD, 1), glm::vec2(1, 1), glm::vec2(1, HALF));

	// y = -1, value 4
	addFace(v, glm::vec3(-1, -1, -1), glm::vec3(-1, -1, +1), glm::vec3(+1, -1, +1), glm::vec3(+1, -1, -1));
	addFace(c, glm::vec2(0, 0), glm::vec2(0, HALF), glm::vec2(THIRD, HALF), glm::vec2(THIRD, 0));

	ArrayBuffer arrayBuffer;
	arrayBuffer.bind();
	glBufferData(GL_ARRAY_BUFFER, v.size()*sizeof(decltype(v[0])), glm::value_ptr(v[0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(vertexIndex);
	glVertexAttribPointer(vertexIndex, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	ArrayBuffer colorBuffer;
	colorBuffer.bind();
	glBufferData(GL_ARRAY_BUFFER, c.size()*sizeof(decltype(c[0])), glm::value_ptr(c[0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(colorIndex);
	glVertexAttribPointer(colorIndex, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	Texture2D texture(loadPNG(getResourcePath("die.png")));
	texture.getGLTexture();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.getGLTexture());
	glUniform1i(textureSamplerIndex, 0 /* Texture unit 0 */);

	m_projectionMatrix = glm::perspective(glm::radians(45.0f),
										  static_cast<float>(nativeWindow.getWidth()) / static_cast<float>(nativeWindow.getHeight()),
										  0.1f, 100.0f);

	m_cubeVertexCount = v.size();

	// target frames per second value
	constexpr unsigned fps = 30;
	const clock::duration framePeriod = std::chrono::milliseconds(static_cast<unsigned>(std::round(1000.0/fps)));

	while (true)
	{
		// limit rendering to requested fps value
		if (m_lastFrameTimeStampValid)
		{
			std::this_thread::sleep_until(m_lastFrameTimeStamp + framePeriod);
		}

		renderFrame();
		nativeWindow.swapBuffers();
	}
}

void DemoRenderer::handleEvent(const MirEvent* event)
{
	const MirEventType type = mir_event_get_type(event);

	if (type == mir_event_type_input)
	{
		const MirInputEvent* inputEvent = mir_event_get_input_event(event);
		if (!inputEvent)
			throw std::runtime_error("mir_event_get_input_event returned null!");

		handleInputEvent(inputEvent);
	}
}

void DemoRenderer::renderFrame()
{
	const clock::time_point t = clock::now();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 view = glm::lookAt(
				glm::vec3(0,0,6),
				glm::vec3(0,0,0),
				glm::vec3(0,1,0)
				);

	// rotate the cube if it has a nonzero speed
	// model this as rotation with fixed angular acceleration (deceleration)
	if (m_lastFrameTimeStampValid && (m_rotationAngularSpeedX != 0.0f || m_rotationAngularSpeedY != 0.0f))
	{
		const std::chrono::duration<float> durationSinceLastFrame = t - m_lastFrameTimeStamp;
		const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(durationSinceLastFrame);
		const float secsSinceLastFrame = ms.count() / 1000.0f;

		rotateAlongAxis(m_rotationAngleX, m_rotationAngularSpeedX, secsSinceLastFrame);
		rotateAlongAxis(m_rotationAngleY, m_rotationAngularSpeedY, secsSinceLastFrame);
	}

	glm::mat4 model = glm::rotate(glm::rotate(glm::mat4(1.0f), m_rotationAngleX, glm::vec3(1, 0, 0)), m_rotationAngleY, glm::vec3(0, 1, 0));

	glm::mat4 mvpMatrix = m_projectionMatrix * view * model;
	glUniformMatrix4fv(m_mvpMatrixIndex, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

	glDrawArrays(GL_TRIANGLES, 0, m_cubeVertexCount);

	m_lastFrameTimeStampValid = true;
	m_lastFrameTimeStamp = t;
}

void DemoRenderer::handleInputEvent(const MirInputEvent* inputEvent)
{
	const MirInputEventType inputType = mir_input_event_get_type(inputEvent);

	switch (inputType)
	{
	case mir_input_event_type_touch:
		{
			const MirTouchEvent* touchEvent = mir_input_event_get_touch_event(inputEvent);
			if (touchEvent)
				handleInputTouchEvent(touchEvent);
		}
		break;

	case mir_input_event_type_key:
		{
			const MirKeyboardEvent* keyboardEvent = mir_input_event_get_keyboard_event(inputEvent);
			if (keyboardEvent)
				handleKeyboardEvent(keyboardEvent);
		}
		break;

	case mir_input_event_type_pointer:
		{
			const MirPointerEvent* pointerEvent = mir_input_event_get_pointer_event(inputEvent);
			if (pointerEvent)
				handlePointerEvent(pointerEvent);
		}
		break;
	}
}

void DemoRenderer::handleInputTouchEvent(const MirTouchEvent* touchEvent)
{
	const unsigned touchCount = mir_touch_event_point_count(touchEvent);
	unsigned touchIndex;

	if (m_pointerState == PointerState::Up)
	{
		for (touchIndex = 0; touchIndex < touchCount; touchIndex++)
		{
			const MirTouchAction action = mir_touch_event_action(touchEvent, touchIndex);
			if (action == mir_touch_action_down)
				break;
		}
		if (touchIndex < touchCount)
		{
			m_pointerState = PointerState::FingerDown;
			m_fingerId = mir_touch_event_id(touchEvent, touchIndex);

			const float pointerX = mir_touch_event_axis_value(touchEvent, touchIndex, mir_touch_axis_x);
			const float pointerY = mir_touch_event_axis_value(touchEvent, touchIndex, mir_touch_axis_y);
			onPointerDown(pointerX, pointerY);
		}
	}
	else if (m_pointerState == PointerState::FingerDown)
	{
		for (touchIndex = 0; touchIndex < touchCount; touchIndex++)
		{
			if (mir_touch_event_id(touchEvent, touchIndex) == m_fingerId)
				break;
		}
		if (touchIndex < touchCount)
		{
			const MirTouchAction action = mir_touch_event_action(touchEvent, touchIndex);
			if (action == mir_touch_action_change)
			{
				const float pointerX = mir_touch_event_axis_value(touchEvent, touchIndex, mir_touch_axis_x);
				const float pointerY = mir_touch_event_axis_value(touchEvent, touchIndex, mir_touch_axis_y);
				onPointerMove(pointerX, pointerY);
			}
			else if (action == mir_touch_action_up)
			{
				m_pointerState = PointerState::Up;

				const float pointerX = mir_touch_event_axis_value(touchEvent, touchIndex, mir_touch_axis_x);
				const float pointerY = mir_touch_event_axis_value(touchEvent, touchIndex, mir_touch_axis_y);
				onPointerUp(pointerX, pointerY);
			}
		}
	}
}

void DemoRenderer::handleKeyboardEvent(const MirKeyboardEvent* keyboardEvent)
{
	const MirKeyboardAction action = mir_keyboard_event_action(keyboardEvent);
	const int keyCode = mir_keyboard_event_scan_code(keyboardEvent);
	const xkb_keysym_t keySym = mir_keyboard_event_key_code(keyboardEvent);

	const char *actionString;
	switch (action)
	{
	case mir_keyboard_action_down:
		actionString = "DOWN";
		break;
	case mir_keyboard_action_up:
		actionString = "UP";
		break;
	case mir_keyboard_action_repeat:
		actionString = "REPEAT";
		break;
	default:
		actionString = "UNKNOWN";
	}

	std::cout << "keyboard event " << actionString << ": key code=" << keyCode << ", key sym=" << keySym << std::endl;
}

void DemoRenderer::handlePointerEvent(const MirPointerEvent* pointerEvent)
{
	const MirPointerAction action = mir_pointer_event_action(pointerEvent);
	const bool primaryButtonDown = mir_pointer_event_button_state(pointerEvent, mir_pointer_button_primary);
	const float pointerX = mir_pointer_event_axis_value(pointerEvent, mir_pointer_axis_x);
	const float pointerY = mir_pointer_event_axis_value(pointerEvent, mir_pointer_axis_y);

	if (m_pointerState == PointerState::Up)
	{
		if (action == mir_pointer_action_button_down && primaryButtonDown)
		{
			m_pointerState = PointerState::PointerDown;
			onPointerDown(pointerX, pointerY);
		}
	}
	else if (m_pointerState == PointerState::PointerDown)
	{
		if ((action == mir_pointer_action_button_up && !primaryButtonDown) || action == mir_pointer_action_leave)
		{
			m_pointerState = PointerState::PointerDown;
			onPointerUp(pointerX, pointerY);
		}
		else if (action == mir_pointer_action_motion)
		{
			onPointerMove(pointerX, pointerY);
		}
	}
}

void DemoRenderer::onPointerDown(float x, float y)
{
	std::cout << "onPointerDown " << x << "," << y << std::endl;

	// stop any posible rotation
	m_rotationAngularSpeedX = 0.0f;
	m_rotationAngularSpeedY = 0.0f;

	// store the start position
	m_lastX = x;
	m_lastY = y;

	// notify gesture handler
	m_swipeGesture.down(x, y);
}

void DemoRenderer::onPointerMove(float x, float y)
{
	std::cout << "onPointerMove " << x << "," << y << std::endl;
	rotateCube(x, y);
}

void DemoRenderer::onPointerUp(float x, float y)
{
	std::cout << "onPointerUp " << x << "," << y << std::endl;
	rotateCube(x, y);
	m_swipeGesture.up(x, y);
}

void DemoRenderer::onSwipe(float dx, float dy)
{
	std::cout << "onSwipe " << dx << "," << dy << std::endl;

	// the rotation axis is perpendicular to the movement -> dx affects rotation along Y and dy affects X
	m_rotationAngularSpeedX = dy / 100.0f; // dy is in screen coordinates, -dy_{gl} = dy_{screen}
	m_rotationAngularSpeedY = dx / 100.0f;
}

void DemoRenderer::rotateCube(float x, float y)
{
	// it would be better to implement something like https://www.khronos.org/opengl/wiki/Object_Mouse_Trackball
	// the following must suffice here :)
	const float dx = x - m_lastX;
	const float dy = m_lastY - y; // y axis coordinate grows downwards, but GLES y coordinate grows upwards (by default)

	m_rotationAngleX += -dy/ 500;
	m_rotationAngleY += dx / 400;

	clampAngle(m_rotationAngleX);
	clampAngle(m_rotationAngleY);

	m_lastX = x;
	m_lastY = y;
}

void DemoRenderer::rotateAlongAxis(float& rotationAngle, float& rotationAngularSpeed, float secsSinceLastFrame)
{
	const float DECELERATION = 2.0f; // rad/s^2

	if (rotationAngularSpeed == 0.0f)
		return; // nothing to do

	const float d = rotationAngularSpeed > 0.0f ? DECELERATION : -DECELERATION;
	const float t_E = rotationAngularSpeed / d; // time when the rotation should stop

	if (secsSinceLastFrame > t_E)
	{
		// the rotation has stopped somewhere between this and the previous frame
		rotationAngle += (rotationAngularSpeed - 0.5f * d * t_E) * t_E;
	}
	else
	{
		// the rotation continues, but the speed is decreased
		rotationAngle += (rotationAngularSpeed - 0.5f * d* secsSinceLastFrame) * secsSinceLastFrame;
		rotationAngularSpeed -= d * secsSinceLastFrame;
	}

	clampAngle(rotationAngle);
}
