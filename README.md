# OpenGL ES Demo Project for Ubuntu SDK

This is a template for a native C++ application using OpenGL ES directly with [Mir](https://launchpad.net/mir). The [glm](https://glm.g-truc.net/) library is used for some GL calculations.

The project has been tested with Ubuntu SDK 15.04 image.

## What does the demo actually do?
If you run it you should see a green background with a 3D die in the center. You can rotate it (sort-of) using touch. Mouse should work too but it has not been tested. You can also try a swipe gesture which makes the cube rotate while slowing down progressively.

## Desktop Kit
As this demo needs Mir it can't be easily ran using the desktop kit.

## How to Use it
### Prerequisites
There are some things that need to be done prior to actually building the project. These include:
* (surprise!) Get Ubuntu SDK installed and running.
* Make sure you have Git installed on your computer. (It doesn't have to be installed inside the Ubuntu SDK containers.) You can verify this by running: `apt install git`

### Building the project
With the prerequisites sorted out the rest is quite easy:

1. Clone this project.
1. Download the sources for the [glm](https://glm.g-truc.net/) library. To do that, open a terminal, go to the root of the project and run `ext/download-dependencies.sh` (Alternatively, CMake could do this step automatically but this would mean git need to be installed in the containers, plus making this step explicit hopefully spares some unnecessary re-downloading.)
1. Start Ubuntu SDK and open the project. To do that, select the menu item _File_/_Open File or Project..._ and choose the `CMakeLists.txt` file in the root folder of the project. Select the kits you want. (You can select all kits with the exception of _Desktop_ (this is the default non-Ubuntu kit) and _UbuntuSDK for desktop_ (it's the Ubuntu desktop kit, but this doesn't include Mir)).
1. Ubuntu SDK IDE runs CMake for the first selected kit. CMake should not produce any error message.
1. Select the kit you want and build the project (_Build_/_Build Project "MirGLESDemo"_). This should finish without errors.
1. Select the start up configuration: Click on the Ubuntu icon in the left bar of Ubuntu SDK IDE. In the menu that appears, in the rightmost column (_Run_), select _MirGLESDemo_. (This is the correct configuration that can be used to launch the application in either the emulator or on a real phone. The name comes from `mainfest.json.in`, from the hooks array. The other option there, _mir_gles_demo_, comes from the name of the actual CMake executable target and it's probably picked up by the default QtCreator CMake plugin. This configuration can't be used to launch the application in the emulator or on the phone.)
1. Run the application, touch the displayed die or drag it with a mouse and enjoy. :)

## License
The sources are licensed under the [GPLv3](https://www.gnu.org/licenses/gpl-3.0.en.html) or later. The [glm](https://glm.g-truc.net/) library is NOT distributed under the GPLv3. See the [license info on its website](https://glm.g-truc.net/copying.txt).
