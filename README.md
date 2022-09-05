# DefEngine (TO BE EDITED)
A simple OpenGL based 2d game engine/libray, with batching support and simple input and collision detection.

## Level Editor
A simple level editor based on layers, you can choose between 5 diferent layers, 0 being the background and 5 the foreground. It's important to note that
entities (like enemies and the player) must be placed on layer 2. If they are placed on a different layer they work like normal tiles, they are just drawn.
The first entity on the list allows you to place static collision regions which are rendered as black semi-transparent squares.

- You can change between layers using the Z and X keys.
- You can move the camera with the arrow keys and zoom in and out with the mouse wheel.
- Tiles and entities cam be removed by right clicking on them.
- To go in and out of test mode press T.

The gray area represents the editable area of the level which at the moment is a static 30x30 tiles. 

![image](https://user-images.githubusercontent.com/66743720/160708817-9a40b290-98df-4489-baf2-2077e759fed5.png)

## Command console
To save and load levels you can open the console by pressing the ESC key while on edit mode. The current commands are:
- save_new LEVEL_NAME  (Save a new level called LEVEL_NAME)
- save                 (Save the progress on the current level)
- load LEVEL_NAME      (Load a level named LEVEL_NAME. The level is looked up at the levels folder)

![image](https://user-images.githubusercontent.com/66743720/160709637-c7a0d718-e0db-4629-a5ae-beb167462739.png)

### TODO 
* Wrap all source code in 'def' namespace.
