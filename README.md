# DefEngine 
A simple OpenGL based 2d game engine/libray, with batching support and simple input and collision detection.

## Level Editor
A simple level editor based on layers, you can choose between 5 diferent layers, 0 being the background and 5 the foreground.
The first entity on the list allows you to place static collision regions which are rendered as black semi-transparent squares.

- You can change between layers using the Q and E keys.
- You can move the camera with the arrow keys and zoom in and out with the mouse wheel.
- Tiles and entities cam be removed by right clicking on them.
- To go in and out of test mode press Space bar.

The blue area represents the editable area of the level.

![DefEngine editor](https://github.com/lRichyl/DefEngine/assets/66743720/c8fb715e-18a2-4329-9f77-8db31fd06b85)

## Command console
To save and load levels you can open the console by pressing the backtick key ` key while on edit mode. The current commands are:
- save_new LEVEL_NAME  (Save a new level called LEVEL_NAME)
- save                 (Save the progress on the current level)
- load LEVEL_NAME      (Load a level named LEVEL_NAME. The level is looked up at the levels folder)

![DefEngine console](https://github.com/lRichyl/DefEngine/assets/66743720/b2be31d6-800d-4890-be44-ac431d54cb20)


### OpenAl dependencies 
1. Go to this website: https://www.openal.org/downloads/
2. 'Select OpenAL 1.1 Windows Installer (zip)'
3. Download will start, extract it and run the installer.
4. Done.

### Build guide
This engine needs to be build using mingw32 (not mingw64) and requires make.
If make is install just run the make command.
In case of any issues run the command 'premake5 gmake2' to re-generate the makefiles, then run the make command.
