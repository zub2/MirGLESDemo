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
#include "MirConnectionWrapper.h"
#include "MirNativeWindow.h"
#include "DemoRenderer.h"

#include <memory>
#include <iostream>
#include <iomanip>

namespace
{
	std::string toString(MirOutputConnectionState connectionState)
	{
		switch (connectionState)
		{
		case mir_output_connection_state_disconnected:
			return "disconnected";
		case mir_output_connection_state_connected:
			return "connected";
		case mir_output_connection_state_unknown:
			return "unknown";
		default:
			return std::string("invalid[") + std::to_string(static_cast<int>(connectionState)) + "]";
		}
	}

	std::string toString(MirOutputType outputType)
	{
		switch (outputType)
		{
		case mir_output_type_unknown:
			return "unknown";
		case mir_output_type_vga:
			return "VGA";
		case mir_output_type_dvii:
			return "DVI-I";
		case mir_output_type_dvid:
			return "DVI-D";
		case mir_output_type_dvia:
			return "DVI-A";
		case mir_output_type_composite:
			return "composite";
		case mir_output_type_svideo:
			return "S-Video";
		case mir_output_type_lvds:
			return "LVDS";
		case mir_output_type_component:
			return "component";
		case mir_output_type_ninepindin:
			return "nine pin DIN";
		case mir_output_type_displayport:
			return "DisplayPort";
		case mir_output_type_hdmia:
			return "HDMI A";
		case mir_output_type_hdmib:
			return "HDMI B";
		case mir_output_type_tv:
			return "TV";
		case mir_output_type_virtual:
			return "virtual";
		case mir_output_type_edp:
			return "EDP";
		default:
			return std::string("invalid[") + std::to_string(static_cast<int>(outputType)) + "]";
		}
	}

	/**
	 * Pick a usable output ID.
	 * This is a bit verbose and also lists the available outputs to stdout.
	 */
	int chooseOutputId(MirConnection* connection)
	{
		std::unique_ptr<MirDisplayConfig, decltype(&mir_display_config_release)>
				displayConfig(mir_connection_create_display_configuration(connection), mir_display_config_release);
		if (!displayConfig)
			throw std::runtime_error("Can't get display configuration.");

		const int numOutputs = mir_display_config_get_num_outputs(displayConfig.get());
		std::cout << "There are " << numOutputs << " outputs available:" << std::endl;

		// find the first connected (or, not-disconnected) && enabled output
		bool haveOutputId = false;
		int selectedOutputId;

		int i = 0;
		while (i < numOutputs)
		{
			MirOutput const* mirOutput = mir_display_config_get_output(displayConfig.get(), i++);

			const int outputId = mir_output_get_id(mirOutput);
			const MirOutputConnectionState connectionState = mir_output_get_connection_state(mirOutput);
			const bool isEnabled = mir_output_is_enabled(mirOutput);
			const MirOutputType outputType  = mir_output_get_type(mirOutput);

			std::cout << "#" << outputId << ": "
					  << toString(outputType) << " "
					  << toString(connectionState) << " "
					  << (isEnabled ? "enabled" : "disabled") << " ";

			MirOutputMode const* outputMode = mir_output_get_current_mode(mirOutput);
			if (outputMode)
			{
				const int width = mir_output_mode_get_width(outputMode);
				const int height = mir_output_mode_get_height(outputMode);
				std::cout << width << u8"\u00d7" << height;
			}
			else
				std::cout << "[mode N/A]";

			std::cout<< std::endl;

			if (!haveOutputId && isEnabled && connectionState == mir_output_connection_state_connected)
			{
				haveOutputId = true;
				selectedOutputId = outputId;
			}
		}

		if (!haveOutputId)
			throw std::runtime_error("Can't find an usable output!");

		return selectedOutputId;
	}

	std::string toString(MirPixelFormat fmt)
	{
		switch (fmt)
		{
		case mir_pixel_format_abgr_8888:
			return "ABGR8888";
		case mir_pixel_format_xbgr_8888:
			return "XBGR8888";
		case mir_pixel_format_argb_8888:
			return "ARGB8888";
		case mir_pixel_format_xrgb_8888:
			return "XRGB8888";
		case mir_pixel_format_bgr_888:
			return "BGR888";
		case mir_pixel_format_rgb_888:
			return "RGB888";
		case mir_pixel_format_rgb_565:
			return "RGB565";
		case mir_pixel_format_rgba_5551:
			return "RGBA5551";
		case mir_pixel_format_rgba_4444:
			return "RGBA4444";
		case mir_pixel_format_invalid:
			return "invalid";
		default:
			return std::string("invalid[") + std::to_string(static_cast<int>(fmt)) + "]";
		}
	}

	void listFormats(MirConnection* connection)
	{
		MirPixelFormat availableFormats[mir_pixel_formats];
		unsigned formatCount;

		mir_connection_get_available_surface_formats(connection, availableFormats,
				sizeof(availableFormats)/sizeof(availableFormats[0]), &formatCount);

		std::cout << "Available formats:";
		for (unsigned i = 0; i < formatCount; i++)
			std::cout << " " << toString(availableFormats[i]);

		if (formatCount == 0)
			std::cout << " NONE";

		std::cout << std::endl;
	}
}

int main()
{
	MirConnectionWrapper mirConnection(nullptr /*default*/, "MirGLESDemo");

	listFormats(mirConnection.get());

	const int outputId = chooseOutputId(mirConnection.get());
	std::cout << "Using output #" << outputId << std::endl;

	MirNativeWindow mirNativeWindow(mirConnection.get(), outputId, "MirGLESDemo Surface", std::make_shared<DemoRenderer>());
	mirNativeWindow.run();

	return 0;
}
