#ifndef CONF_H
#define CONF_H

//#define MAC
#define SHADERS
//#define EXTERNAL_GEOMETRY

enum {
	STATE_IDLE,
	STATE_DRAW_CUBE, // rysowanie obiektu
	STATE_DRAW_CLOWN, //@ Rysujem klauna :DDD
	STATE_MOVE_HEAD, //@ Stan zmiany jakie� zmiennej
	STATE_ROTATE_CUBE, // obrot obiektu za pomoca klawiatury
	STATE_DRAW_OBSTACLES,
	STATE_MOVE_FORWARD,
	STATE_MOVE_BACKWARD,
	STATE_MOVE_LEFT,
	STATE_MOVE_RIGHT,
	STATE_CHECK_COLLISION,
	STATE_CHANGE_ROTATION_RIGHT,
	STATE_CHANGE_ROTATION_LEFT,
	STATE_DRAW_BOARD,
	STATE_CAMERA,
	STATE_SCENE_LIGHT,
	STATE_DRAW_TEAPOT,
	STATE_TEXTURE,
	STATE_FIRE,
	STATE_CARRYING_CUBE
};



#endif