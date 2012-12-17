
#include "conf.h"

#include <stdio.h>

#ifdef MAC
#include <GLUT/glut.h> 
#else
#include <windows.h>
#include "../3rd/glew/glew.h"
#include "../3rd/glut/glut.h"
#pragma warning(disable:4996) // Wczytuje kod z poprzednich kompilacj / Je¿eli nie wczyta bibliotek
#endif

#include "gl.h"

void idle(void) {
	cgl.idle();
}

void display(void) {
	cgl.display();
}

void keyboard_press( unsigned char key,int a,int b ) {
	cgl.keyboardPress( key, a, b );
}

void keyboard_up( unsigned char key,int a,int b ) {
	cgl.keyboardUp( key, a, b );
}


void timer( int flag ) {
	cgl.timer();	
	glutTimerFunc( CGL::TIMER, timer, 0 );
}


void mouse_move( int x, int y ) {
	cgl.mouseMotion( x, y );
}

void mouse_press( int button, int state, int x, int y ) {
	cgl.mousePress( button, state, x, y );
}

int main(int argc, char* argv[]) {

	glutInit(&argc, argv);
 	glutInitWindowSize(768,768); // Tu sobie ustalamy rozmiar okna
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA); 
	glutInitWindowPosition( 1440-768-10, 10 );
	glutCreateWindow("Lol Game");	

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard_press);
	glutKeyboardUpFunc(keyboard_up);
	glutTimerFunc( 1000, timer, 0 );
	glutMotionFunc( mouse_move );
	glutMouseFunc( mouse_press );

#ifndef MAC
	GLint GlewInitResult = glewInit();
	if (GlewInitResult != GLEW_OK) {
		printf("ERROR: %s\n", glewGetErrorString(GlewInitResult));
	}
#endif

	cgl.init();
	
	// dodatkowe zrodlo swiatla przemieszczane za pomoca myszy
	cgl.state_list.push_back( STATE_SCENE_LIGHT );

	//cgl.state_list.push_back( STATE_DRAW_BOARD );
	
	cgl.state_list.push_back( STATE_DRAW_OBSTACLES );
	//@ Odpalamy stan zmiany zmiennej
	//cgl.state_list.push_back( STATE_MOVE_HEAD );
	// rysowanie szescianu
	//cgl.state_list.push_back( STATE_DRAW_CUBE );
	cgl.state_list.push_back( STATE_DRAW_TEAPOT );
	//cgl.state_list.push_back( STATE_DRAW_CLOWN );

	
	// cieniowanie phonga
	//cgl.state_list.push_back(STATE_SCENE_LIGHT);	
	
	glutMainLoop();

	return 0;
}
