
#include "conf.h"

#ifdef MAC
#include <GLUT/glut.h>
#else
#include "../3rd/glew/glew.h"
#include "../3rd/glut/glut.h"
#include <windows.h>
#endif

#include <stdio.h>
#include <iostream>
#include <list>
#include <algorithm>
#include <time.h>
#include <math.h>

using namespace std;

#include "shaders.h"
#include "util.h"
#include "gl.h"
#include "Obstacle.h"

//#define ASE

CGL cgl;


///////////////////////////////////////// inicjalizacja srodowiska graficznego
void CGL::init( void ) {

	
	// ustawienie parametrow kamery 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective( 100, 1, 1, 1000 );
	gluLookAt( 0, 0, 7, 0, 0, 0, 0, 1, 0 );

	// wyzerowanie transformacji modelu
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// wlaczenie bufora Z
	glEnable( GL_DEPTH_TEST ); 
	
	// wlaczenie automatycznej normalizacji wektorow po skalowaniu
	glEnable( GL_NORMALIZE ); 

	// ustawienie parametrow zrodla swiatla, wlaczenie oswietlenia
	float g_lightPos[4] = { 0.0f, 0.0f, 7.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, g_lightPos); 
	GLfloat color[] = { 0.8f, 0.8f, 0.8f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, color);
	glLightfv(GL_LIGHT0, GL_AMBIENT, color);
	glLightfv(GL_LIGHT0, GL_EMISSION, color);
	glLightfv(GL_LIGHT0, GL_SPECULAR, color);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	glEnable(GL_LIGHTING);

	// ustawienie koloru tla
	glClearColor( 0.2f, 0.2f, 0.2f, 1.0f ); 
	
	// wlaczenie cieniowania Gouraud'a (smooth shading)
	glShadeModel( GL_SMOOTH ); 

	// back-face culling
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	// inicjalizacja shaderow (cieniowanie Phong'a)
#ifdef SHADERS
	shaderprogram = CShader::init();
	if( shaderprogram > 0 )
		printf( "shaders initialised\n" );
#endif	
	

#ifndef MAC
	// wczytywanie tekstury z pliku dyskowego
	BITMAPINFOHEADER bitmapInfoHeader;
	
	unsigned char* bitmap_data = CUTIL::LoadBitmapFile( "tex.bmp", &bitmapInfoHeader ); 
	
	if( bitmap_data != NULL ) {
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmapInfoHeader.biWidth,
					 bitmapInfoHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmap_data);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glEnable(GL_TEXTURE_2D);
	}
#endif
	
	// wczytanie obiektu ASE z pliku dyskowego
#ifdef EXTERNAL_GEOMETRY
	aseGeometry = CUTIL::loadASE( "chillmod.ase" );
#endif
	
	// wczytanie danych planszy gry
	board = read_board();
	
	board_to_obstacles( board );

	map_w = board_w;
	map_h = board_h;
	
}


///////////////////////////////////////// glowna procedura przerysowania ekranu
void CGL::display( void ) {

	//char* version = (char*)glGetString(GL_VERSION);
	//printf("version(%s)\n", version);
	
	// obsluga klawiatury	
	keyOperations(); 
	
#ifdef SHADERS
	// wywolanie shaderow lub standardego renderera
	if( flag_phong )
		glUseProgram(shaderprogram);
	else
		glUseProgram(0);	
#endif

	// aktualny czas
	float now = getTime();

	// wyczyszczenie bufora ramki (ang. frame buffer)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	
	
	glPushMatrix();
	
#ifdef SHADERS
	if( flag_phong ) {
		glMaterialfv( GL_FRONT, GL_DIFFUSE, colorDim );
		glMaterialfv( GL_FRONT, GL_AMBIENT, colorDim );
		glMaterialfv( GL_FRONT, GL_EMISSION, colorGray );
		glMaterialfv( GL_FRONT, GL_SPECULAR, colorDim );
		CUTIL::renderBitmapString( -3, 3, GLUT_BITMAP_HELVETICA_18, (char*)"Phong shading xD" );
	}
#endif
	
				int x, y;
	
	// glowna petla obslugujaca stany gry
	list<int> states = state_list;
	list<int>::iterator state;
	
	list<Obstacle*> objects = objects_list;
	list<Obstacle*>::iterator object_it;
	
	for( state = states.begin(); state != states.end(); ++state) {
	
	
		switch( *state ) {
	
			// rysowanie szescianu	
			case STATE_DRAW_CUBE:
				draw_cube();
				break;				
				
			case STATE_DRAW_CLOWN:
				draw_clown( headX );
				break;

			case STATE_MOVE_HEAD:
				// Dodajemy lub odejmujemy od naszej zmiennej w zale¿noœci od 'trybu'
				headX += changeX? 0.06: -0.06;
				// Je¿eli zmienna wychodzi poza zakres
				if( headX > 0.5 || headX < -0.5 )
					changeX = !changeX;
				break;

			case STATE_DRAW_OBSTACLES:
				draw_obstacles();
				break;
			
			case STATE_MOVE_FORWARD:
			case STATE_MOVE_BACKWARD:
				moveY_change = moveY_value * ( now - moveF_time_start ) / ( moveF_time_end - moveF_time_start );
				moveX_change = moveX_value * ( now - moveF_time_start ) / ( moveF_time_end - moveF_time_start );
				if( now > moveF_time_end ) {
					state_list.remove( STATE_MOVE_FORWARD );
					state_list.remove( STATE_MOVE_BACKWARD );
					//state_list.remove( STATE_CHECK_COLLISION );
				}
				break;
			
			case STATE_MOVE_LEFT:
			case STATE_MOVE_RIGHT:
				moveY_change = moveY_value * ( now - moveS_time_start ) / ( moveS_time_end - moveS_time_start );
				moveX_change = moveX_value * ( now - moveS_time_start ) / ( moveS_time_end - moveS_time_start );
				if( now > moveS_time_end ) {
					state_list.remove( STATE_MOVE_LEFT );
					state_list.remove( STATE_MOVE_RIGHT );
					//state_list.remove( STATE_CHECK_COLLISION );
				}
				break;
			
			case STATE_CHECK_COLLISION:
				collision = false;/*
				for( object_it = objects.begin(); object_it != objects.end(); object_it++ ) {
					//if( obiekt_za_daleko_od_gracza )
					//	continue;
					collisionY = false;
					collisionX = false;
					if( moveX > (*object_it)->getX() && (*object_it)->getX() + (*object_it)->getWidth() > moveX )
						collisionX = true;
					if( moveY > (*object_it)->getY() && (*object_it)->getY() + (*object_it)->getHeight() > moveY )
						collisionY = true;
					if( collisionX && collisionY )
						collision = true;
				}//*/
				x = board_w * ( moveX + map_w/2 ) / (map_w-1);
				y = board_h * ( moveY + map_h/2 ) / (map_h-1);
				//gy = y * block_size - range - (float)block_size/2.0f;
				//if( board[y][x] )
				//	collision = true;
				//cout << x << ", " << y << " | " << moveX << ", " << moveY << endl;
				
				cout << endl;
				cout << board[y+1][x-1] << board[y+1][x] << board[y+1][x+1] << endl;
				cout << board[y  ][x-1] << board[y  ][x] << board[y  ][x+1] << endl;
				cout << board[y-1][x-1] << board[y-1][x] << board[y-1][x+1] << endl;
				
				//cout << "pos: " << moveX << ", " << moveY
				//	<< " collision: " << collisionX << ", " << collisionY << endl;
				if( !collision /*&& -range < moveX_change + moveX_start && moveX_change + moveX_start < range*/ )
					moveX = moveX_start + moveX_change;
				if( !collision /*&& -range < moveY_change + moveY_start && moveY_change + moveY_start < range*/ )
					moveY = moveY_start + moveY_change;
				//if( collision )
				//	state_list.remove( STATE_CHECK_COLLISION );
				break;
				
			case STATE_TEXTURE:
				draw_cube_texture( rotateY );
				break;				
				
			case STATE_CAMERA:
				glTranslatef( translateX, translateY, 0.0f );
				break;

			case STATE_DRAW_TEAPOT:
				draw_teapot( rotateY );
				break;				
				
			case STATE_ROTATE_CUBE:
				draw_cube( rotateY );
				break;
			
			case STATE_DRAW_BOARD:
				draw_board();
				break;
			
			case STATE_CHANGE_ROTATION_RIGHT:
				
				rotateY = rotateY_start - rotateY_value * ( now - rotateY_time_start ) / ( rotateY_time_end - rotateY_time_start );
				
				if( rotateY < 0.0f )
					rotateY = rotateY + 360.0f;
				
				if ( now > rotateY_time_end ) {
					state_list.remove( STATE_CHANGE_ROTATION_RIGHT );
				}
				
				break;
				
			case STATE_CHANGE_ROTATION_LEFT:
				
				rotateY = rotateY_start + rotateY_value * ( now - rotateY_time_start ) / ( rotateY_time_end - rotateY_time_start );
				
				if( rotateY >= 360.0f )
					rotateY = 0.0f;
				
				if ( now > rotateY_time_end ) {
					state_list.remove( STATE_CHANGE_ROTATION_LEFT );
				}
				
				break;	
				
				
			/** zmiana polozenia zrodla swiatla za pomoca myszy
			 (translateX, translateY) - wartosc przesuniecia
			 */
			case STATE_SCENE_LIGHT:
				
				glLightfv(GL_LIGHT1, GL_AMBIENT, colorWhite ); // Rozmyte, jak przez chmury
				glLightfv(GL_LIGHT1, GL_DIFFUSE, colorWhite ); // Jak s³oñce
				glLightfv(GL_LIGHT1, GL_EMISSION, colorWhite); // Œwiat³o odbite równoczeœnie we wszystkich kierunkach
				glLightfv(GL_LIGHT1, GL_SPECULAR, colorWhite); // Te¿ punktowe, ale uwzglêdnia odbicia
				glEnable(GL_LIGHT1);
	
				glPushMatrix();
				
				glTranslatef( translateX, translateY, 1.0f);
				
				float g_lightPos[4] = { 0.0f, 1.5f, 1.0f, 1.0f };
				glLightfv(GL_LIGHT1, GL_POSITION, g_lightPos); 
						

				// rysowanie polozenia zrodla swiatla
				glTranslatef(g_lightPos[0], g_lightPos[1], g_lightPos[2]);
				glScalef( 0.1f, 0.1f, 0.1f );
				
				float color_white[] = { 0.2f, 0.2f, 0.2f };
				glMaterialfv( GL_FRONT, GL_AMBIENT, color_white);
				glMaterialfv( GL_FRONT, GL_EMISSION, color_white);
				
				glutSolidSphere(1, 20, 20);
				
				glPopMatrix();	
				
				break;

		}
		
	}
	
	glPopMatrix();
	
	// wyswietlenie tylnego ekranu
	glutSwapBuffers(); 
	
	// wymyszenie przerysowania podczas nastêpnej synchronizacji ekranu
	glutPostRedisplay(); 
}


///////////////////////////////////////// rysowanie szescianu
void CGL::draw_cube( void ) {
	
	// kolor obiektu
	glMaterialfv( GL_FRONT, GL_DIFFUSE, colorGreen );
	glMaterialfv( GL_FRONT, GL_AMBIENT, colorDim );
	glMaterialfv( GL_FRONT, GL_EMISSION, colorDim );
	glMaterialfv( GL_FRONT, GL_SPECULAR, colorDim );
	
	// polozenie i ksztalt obiektu
	glPushMatrix();
	
	glRotatef( 10.0f, 0.0f, 0.0f, 1.0f );
	glRotatef( 20.0f, 1.0f, 0.0f, 0.0f );
	glRotatef( -30.0f, 0.0f, 1.0f, 0.0f );
	
	glScalef( 0.8f, 0.8f, 0.8f );
	
	CUTIL::drawBox();
	//glutSolidTeapot( 0.7 );
	//glutSolidSphere( 1.5, 20, 20 );
	//glutSolidCube(2);
	
	glPopMatrix();
}


void CGL::draw_clown( float headX ) {
	
	glPushMatrix();

	glMaterialfv( GL_FRONT, GL_DIFFUSE, colorGreen );
	glMaterialfv( GL_FRONT, GL_AMBIENT, colorDim );
	glMaterialfv( GL_FRONT, GL_EMISSION, colorDim );
	glMaterialfv( GL_FRONT, GL_SPECULAR, colorDim );

	glTranslatef( moveX, moveY, 0 );
	glRotatef( rotateY, 0, 0, 1 );
	glRotatef( 90, 1, 0, 0 );
	glScalef( 0.1, 0.1, 0.1 );

	// G³owa :D
	glPushMatrix();
	glTranslatef( 0, 2.2, -1 );
	//glRotatef( 0, 0, 0, 0 );
	//glScalef( 0, 0, 0 );
	glutSolidSphere( 1, 20, 20 );
	glPopMatrix();//*/
	
	// Tu³ów :)
	glPushMatrix();
	glTranslatef( 0, -0.5, -1 );
	//glRotatef( 0, 0, 0, 0 );
	glScalef( 1, 1.5, 1 );
	glutSolidCube( 2 );
	//CUTIL::drawBox();
	glPopMatrix();
	
	// Prawa rêka ;p
	glPushMatrix();
	//@ Ustawiamy sobie wzglêdny punkt obrotu
	glTranslatef( -2, 1.2, -1 );
	//@ Obrcamy rêkê uwzglêdniaj¹c zmienn¹
	glRotatef( -30 + headX*20, 0, 0, 1 );
	glTranslatef( -0.5, -0.2, 0 );
	glScalef( 1.5, 1, 1 );
	glutSolidCube( 1.5 );
	glPopMatrix();
	
	// Lewa rêka ;p
	glPushMatrix();
	glTranslatef( 2.5, 1.2, -1 );
	glRotatef( 30, 0, 0, 1 );
	glScalef( 1.5, 1, 1 );
	glutSolidCube( 1.5 );
	glPopMatrix();
	
	// Prawa noga ;o
	glPushMatrix();
	glTranslatef( -1.8, -3.2, -1 );
	glRotatef( 60, 0, 0, 1 );
	glScalef( 1.5, 1, 1 );
	glutSolidCube( 1.5 );
	glPopMatrix();
	
	// Prawa noga ;o
	glPushMatrix();
	glTranslatef( 1.8, -3.2, -1 );
	glRotatef( -60, 0, 0, 1 );
	glScalef( 1.5, 1, 1 );
	glutSolidCube( 1.5 );
	glPopMatrix();

	glPopMatrix();

}

///////////////////////////////////////// rysowanie szescianu z tekstur¹
void CGL::draw_cube_texture( float angle ) {
	
	// kolor obiektu
	glMaterialfv( GL_FRONT, GL_DIFFUSE, colorGreen );
	glMaterialfv( GL_FRONT, GL_AMBIENT, colorDim );
	glMaterialfv( GL_FRONT, GL_EMISSION, colorDim );
	glMaterialfv( GL_FRONT, GL_SPECULAR, colorDim );
	
	// polozenie i ksztalt obiektu
	glPushMatrix();
	
	glRotatef( 10.0f, 0.0f, 0.0f, 1.0f );
	glRotatef( 20.0f, 1.0f, 0.0f, 0.0f );
	glRotatef( angle, 0.0f, 1.0f, 0.0f );
	
	glScalef( 2.0f, 2.0f, 2.0f );
	
	glBindTexture( GL_TEXTURE_2D, texture );
	
	CUTIL::drawBox();
	
	glPopMatrix();
	
	glBindTexture( GL_TEXTURE_2D, 0 );
}

///////////////////////////////////////// rysowanie szescianu
void CGL::draw_cube( float angle ) {

	// kolor obiektu
	glMaterialfv( GL_FRONT, GL_DIFFUSE, colorGreen );
	glMaterialfv( GL_FRONT, GL_AMBIENT, colorDim );
	glMaterialfv( GL_FRONT, GL_EMISSION, colorDim );
	glMaterialfv( GL_FRONT, GL_SPECULAR, colorDim );

	// polozenie i ksztalt obiektu
	glPushMatrix();
	
	glRotatef( 10.0f, 0.0f, 0.0f, 1.0f );
	glRotatef( 20.0f, 1.0f, 0.0f, 0.0f );
	glRotatef( angle, 0.0f, 1.0f, 0.0f );
	
	glScalef( 2.0f, 2.0f, 2.0f );

	CUTIL::drawBox();
	
	glPopMatrix();
}

///////////////////////////////////////// rysowanie czajnika
void CGL::draw_teapot( float angle ) {
	
	// kolor obiektu
	float col[4];
	col[3] = 1.0f;
	
	col[0] = col[1] = col[2] = 0.001f;
	glMaterialfv( GL_FRONT, GL_EMISSION, col);
	glMaterialfv( GL_FRONT, GL_SPECULAR, col);
	glMaterialfv( GL_FRONT, GL_DIFFUSE, col);	
	
	col[1] = 0.3f;
	glMaterialfv( GL_FRONT, GL_AMBIENT, col);

	if( flag_diffuse ) {
		col[1] = 1.0f;
		glMaterialfv( GL_FRONT, GL_DIFFUSE, col);
	}
	
	if( flag_specular ) {
		col[0] = col[1] = col[2] = 1.0f;
		glMaterialfv( GL_FRONT, GL_SPECULAR, col);
		glMateriali(GL_FRONT, GL_SHININESS, 20);
	}
	
	// polozenie i ksztalt obiektu
	glPushMatrix();
	
	glTranslatef( moveX, moveY, 0 );
	//glRotatef( 10.0f, 0.0f, 0.0f, 1.0f );
	//glRotatef( 20.0f, 1.0f, 0.0f, 0.0f );
	glRotatef( angle, 0.0f, 0.0f, 1.0f );
	glRotatef( 90.0f, 1.0f, 0.0f, 0.0f );
	glRotatef( 90.0f, 0.0f, 1.0f, 0.0f );
	glScalef( 0.1f, 0.1f, 0.1f );
	
	glScalef( 2.0f, 2.0f, 2.0f );
	
	glutSolidTeapot(1);
	
	//glutSolidCube(1);
	
	//CUTIL::drawBox();
	
	glPopMatrix();
}

///////////////////////////////////////// rysowanie planszy
void CGL::draw_board( void ) {
		
	glPushMatrix();
	
	glMaterialfv( GL_FRONT, GL_DIFFUSE, colorBlack );
	glMaterialfv( GL_FRONT, GL_AMBIENT, colorDim );
	glMaterialfv( GL_FRONT, GL_EMISSION, colorDim );
	glMaterialfv( GL_FRONT, GL_SPECULAR, colorDim );

	glTranslatef( 0.0f, 0.0f, -1.0f );
	glScalef( board_w, board_h, 0.1f );
	glutSolidCube( 1.0f );
	
	glPopMatrix();
	
	if (board == NULL) {
		printf( "ERROR: cannot read the BOARD data.\n" );
		return;
	}

	float col[4] = { 0.1f, 1.0f, 0.1f, 1.0f };
	glMaterialfv( GL_FRONT, GL_DIFFUSE, col);
	
	glPushMatrix();
	glScalef( map_w, map_h, 1.0f );

	for (int y = 0; y < board_h; y++) {
		for (int x = 0; x < board_w; x++) {
			if (board[y][x] == 1) {
				glPushMatrix();
				glTranslatef(
					(float)x / (float)board_w - 0.5f,
					(float)y / (float)board_h - 0.5f, 0.0f );
				glScalef(
					1.0f / (float)board_w,
					1.0f / (float)board_h, 0.4f );
				glutSolidCube( 1.0f );
				glPopMatrix();	
			}
		}
	}
	glPopMatrix();
	
}

void CGL::draw_obstacles() {
	
	list<Obstacle*> objects = objects_list;
	list<Obstacle*>::iterator object_it;

	float col[4] = { 0.1f, 0.1f, 0.8f, 0.03f };
	glMaterialfv( GL_FRONT, GL_DIFFUSE, col);
	//glPushMatrix();
	//glScalef( 10, 6, 10 );
	for( object_it = objects.begin(); object_it != objects.end(); object_it++ )
		(*object_it)->draw();
	//glPopMatrix();
}


///////////////////////////////////////// IDLE
void CGL::idle( void ) {

}

///////////////////////////////////////// obsluga klawiatury

void CGL::keyboardUp( unsigned char key,int a,int b ) {
	
	keyStates[key] = false;
}


void CGL::keyboardPress( unsigned char key,int a,int b ) {

	keyStates[key] = true;
}

	
void CGL::keyOperations( void ) {	
	
	
	if ( keyStates[27] ) {
		exit(0);
	}
	
	if ( keyStates['q'] ) {
		if (!findState(STATE_CHANGE_ROTATION_LEFT)) {
			state_list.remove( STATE_CHANGE_ROTATION_RIGHT );
			state_list.push_front( STATE_CHANGE_ROTATION_LEFT );
			rotateY_start = rotateY;
			rotateY_value = 20;
			rotateY_time_start = getTime();
			rotateY_time_end = rotateY_time_start + 100;
		}
	}	
	
	if ( keyStates['e'] ) {
		if (!findState(STATE_CHANGE_ROTATION_RIGHT)) {
			state_list.remove( STATE_CHANGE_ROTATION_LEFT );
			state_list.push_front( STATE_CHANGE_ROTATION_RIGHT );
			rotateY_start = rotateY;
			rotateY_value = 20;
			rotateY_time_start = getTime();
			rotateY_time_end = rotateY_time_start + 100;
		}
	}

	if ( keyStates['z'] ) {
		cameraX = cameraX - 0.1;
	}
	
	if ( keyStates['x'] ) {
		cameraX = cameraX + 0.1;
	}
	/*
	if ( keyStates['s'] ) {
		flag_specular = !flag_specular;
	}
	
	if ( keyStates['d'] ) {
		flag_diffuse = !flag_diffuse;
	}//*/
	
	if ( keyStates['p'] ) {
		flag_phong = !flag_phong;
	}		
	
	if( keyStates['w'] ) {
		if( !findState( STATE_MOVE_FORWARD ) ) {
			state_list.remove( STATE_MOVE_BACKWARD );
			state_list.push_front( STATE_MOVE_FORWARD );
			moveX_start = moveX;
			moveY_start = moveY;
			moveX_value = cos( ( M_PI * rotateY ) / 180.0f + M_PI/2 ) * 0.3f;
			moveY_value = sin( ( M_PI * rotateY ) / 180.0f + M_PI/2 ) * 0.3f;
			moveF_time_start = getTime();
			moveF_time_end = moveF_time_start + 100;
			move = true;
		}
	}		
	
	if( keyStates['s'] ) {
		if( !findState( STATE_MOVE_BACKWARD ) ) {
			state_list.remove( STATE_MOVE_FORWARD );
			state_list.push_front( STATE_MOVE_BACKWARD );
			moveX_start = moveX;
			moveY_start = moveY;
			moveX_value = cos( ( M_PI * rotateY ) / 180.0f - M_PI/2 ) * 0.3f;
			moveY_value = sin( ( M_PI * rotateY ) / 180.0f - M_PI/2 ) * 0.3f;
			moveF_time_start = getTime();
			moveF_time_end = moveF_time_start + 100;
			move = true;
		}
	}		
	
	if( keyStates['a'] ) {
		if( !findState( STATE_MOVE_LEFT ) ) {
			state_list.remove( STATE_MOVE_RIGHT );
			state_list.push_front( STATE_MOVE_LEFT );
			moveX_start = moveX;
			moveY_start = moveY;
			moveX_value = cos( ( M_PI * rotateY ) / 180.0f + M_PI ) * 0.3f;
			moveY_value = sin( ( M_PI * rotateY ) / 180.0f + M_PI ) * 0.3f;
			moveS_time_start = getTime();
			moveS_time_end = moveS_time_start + 100;
			move = true;
		}
	}		
	
	if( keyStates['d'] ) {
		if( !findState( STATE_MOVE_RIGHT ) ) {
			state_list.remove( STATE_MOVE_LEFT );
			state_list.push_front( STATE_MOVE_RIGHT );
			moveX_start = moveX;
			moveY_start = moveY;
			moveX_value = cos( ( M_PI * rotateY ) / 180.0f ) * 0.3f;
			moveY_value = sin( ( M_PI * rotateY ) / 180.0f ) * 0.3f;
			moveS_time_start = getTime();
			moveS_time_end = moveS_time_start + 100;
			move = true;
		}
	}

	if( move && !findState( STATE_CHECK_COLLISION ) ) {
		cgl.state_list.push_front( STATE_CHECK_COLLISION );
		move = false;
	}

	//if( keyStates[']'] ) {
	//}

}


///////////////////////////////////////// timer
void CGL::timer( void ) {

}

///////////////////////////////////////// obsluga ruchu myszy
void CGL::mouseMotion( int x, int y ) {

	if ( mouse_button == 1 ) {

		GLint param[4];
		glGetIntegerv( GL_VIEWPORT, param );
		
		translateX = mouse_move_x + (float)( x - mouse_x ) / (float)(param[2] - param[0]) * 10.0f;
		translateY = mouse_move_y + (float)( mouse_y - y ) / (float)(param[3] - param[1]) * 10.0f;
		
	}
	

}

///////////////////////////////////////// obsluga wcisniecia klawisza myszy
void CGL::mousePress( int button, int state, int x, int y ) {

	if( button == GLUT_LEFT_BUTTON ) {
		
		mouse_button = 1;
		mouse_x = x;
		mouse_y = y;	
		mouse_move_x = translateX;
		mouse_move_y = translateY;
	}
	else {
		mouse_button = 0;
	}
}

///////////////////////////////////////// zwraca aktualny czas systemowy w milisekundach
float CGL::getTime( void ) {

#ifdef MAC
	return (float)clock() * 100000.0f / (float)CLOCKS_PER_SEC;
#else
	return (float)clock() * 1000.0f / (float)CLOCKS_PER_SEC;
#endif
}

///////////////////////////////////////// wczytanie struktury planszy gry z pliku
int** CGL::read_board( void ) {

	FILE * pFile;
	pFile = fopen ("board.txt","rt");
	
	if (pFile == NULL) {
		return NULL;
	}
	
	//int board_w, board_h;
	
	fscanf(pFile, "%d,%d\n", &board_w, &board_h );	
	printf( "board size: %d,%d\n", board_w, board_h );

	int val;
	
	int** board = new int*[board_h];
	
	for (int y = 0; y < board_h; y++) {
		
		board[y] = new int[board_w];
		
		for (int x = 0; x < board_w; x++) {
			if( fscanf( pFile, "%d,", &val ) == EOF )
				return NULL;
			
			board[y][x] = val;
		}
	}	

	fclose (pFile);
	
	return board;
}

void CGL::create_obstacle( float x, float y, float width, float height ) {
	Obstacle * object = new Obstacle( x, y, width, height );
	cgl.objects_list.push_back( object );
}

void CGL::board_to_obstacles( int ** board )
{
	float gx, gy;
	float block_size = 1.0;
	for( int y = 0; y < board_h; y++ )
		for( int x = 0; x < board_w; x++ )
			if( board[y][x] ) {
				gx = x * block_size - range - (float)block_size/2.0f;
				gy = y * block_size - range - (float)block_size/2.0f;
				Obstacle * object = new Obstacle( gx, gy, block_size, block_size );
				cgl.objects_list.push_back( object );
			}
}


void info( char * text ) {
	glMaterialfv( GL_FRONT, GL_DIFFUSE, colorDim );
	glMaterialfv( GL_FRONT, GL_AMBIENT, colorDim );
	glMaterialfv( GL_FRONT, GL_EMISSION, colorGray );
	glMaterialfv( GL_FRONT, GL_SPECULAR, colorDim );
	CUTIL::renderBitmapString( -3, 3, GLUT_BITMAP_HELVETICA_18, text );
}
