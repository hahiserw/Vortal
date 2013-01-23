
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

//#include <unistd.h>

using namespace std;

#include "shaders.h"
#include "util.h"
#include "gl.h"

#include "Obstacle.h"
#include "Cube.h"
#include "Shoot.h"
#include "ExitDoor.h"
#include "Button.h"

//#define ASE

CGL cgl;


///////////////////////////////////////// inicjalizacja srodowiska graficznego
void CGL::init( void ) {

	
	// ustawienie parametrow kamery 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective( 100, 1, 0.1f, 1000 );
	//gluLookAt( 0, 0, 7, 0, 0, 0, 0, 1, 0 );

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
	/*/ wczytywanie tekstury z pliku dyskowego
	BITMAPINFOHEADER bitmapInfoHeader;
	
	unsigned char* bitmap_data = CUTIL::LoadBitmainput( "tex.bmp", &bitmapInfoHeader ); 
	
	if( bitmap_data != NULL ) {
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmapInfoHeader.biWidth,
					 bitmapInfoHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmap_data);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glEnable(GL_TEXTURE_2D);
	}//*/
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

	Obstacle *obstacle;

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

	/*
	glTranslatef( 0.0f, 0.0f, 0.6f ); // Wysokoœæ kamery
	glRotatef( -30, 1.0f, 0.0f, 0.0f );
	glRotatef( -rotation, 0.0f, 0.0f, 1.0f );
	glTranslatef( -transX, -transY, 0.0f );
	*/
	
#ifdef SHADERS
	if( flag_phong ) {
		glMaterialfv( GL_FRONT, GL_DIFFUSE, colorDim );
		glMaterialfv( GL_FRONT, GL_AMBIENT, colorDim );
		glMaterialfv( GL_FRONT, GL_EMISSION, colorGray );
		glMaterialfv( GL_FRONT, GL_SPECULAR, colorDim );
		CUTIL::renderBitmapString( -3, 3, GLUT_BITMAP_HELVETICA_18, (char*)"Phong shading xD" );
	}
#endif
	
	//int x, y;
	
	// glowna petla obslugujaca stany gry
	list<int> states = state_list;
	list<int>::iterator state;
	
	list<Obstacle*> objects = objects_list;
	list<Obstacle*>::iterator object_it;
	
	list<Shoot*> shoots = shoots_list;
	list<Shoot*>::iterator shoot_it;

	
	for( state = states.begin(); state != states.end(); ++state) {
	
	
		switch( *state ) {

			case MENU:
				camR = M_PI * ( (int)now % 5000 ) / 2500.0f;// - 1.0f;
				camX = cos( camR ) * 10.0f;
				camY = sin( camR ) * 10.0f;
				gluLookAt( camX, camY, 5.0f,
					0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f );
				glPushMatrix();
				glTranslatef( camX *0.96f, camY*0.96f, 5.05f );
				glRotatef( 180*camR/M_PI, 0.0f, 0.0f, 1.0f );
				glScalef( 0.1f, 2.0f, 1.0f );
				// Teksturka z menu, czy coœ
				glutSolidCube( 0.2f );
				glPopMatrix();
				state_list.remove( STATE_MOVE_FORWARD );
				state_list.remove( STATE_MOVE_BACKWARD );
				state_list.remove( STATE_MOVE_LEFT );
				state_list.remove( STATE_MOVE_RIGHT );
				state_list.remove( STATE_CHANGE_ROTATION_RIGHT );
				state_list.remove( STATE_CHANGE_ROTATION_LEFT );
				break;
				/*
			case GAME:
				if( menu )

				break;*/

			case MOVE_CAMERA:
				gluLookAt( moveX, moveY, 1.0f,
					moveX + cos( ( M_PI * rotateY ) / 180.0f + M_PI/2 ) * 0.3f,
					moveY + sin( ( M_PI * rotateY ) / 180.0f + M_PI/2 ) * 0.3f,
					1.0f,
					0.0f, 0.0f, 1.0f );
				break;
			// rysowanie szescianu	
			case STATE_DRAW_CUBE:
				draw_cube();
				break;				
				
			case STATE_DRAW_CLOWN:
				draw_clown( headX );
				break;

			case STATE_DRAW_OBSTACLES:
				draw_obstacles();
				break;

			case STATE_CARRYING_CUBE:
				//cube->moveTo( moveX, moveY );
				break;
			
			case STATE_MOVE_FORWARD:
			case STATE_MOVE_BACKWARD:
				collision = false;
				moveY_change = moveY_value * ( now - moveF_time_start ) / ( moveF_time_end - moveF_time_start );
				moveX_change = moveX_value * ( now - moveF_time_start ) / ( moveF_time_end - moveF_time_start );//*
				for( object_it = objects.begin(); object_it != objects.end(); object_it++ ) {
					obstacle = *object_it;
					//if( obiekt_za_daleko_od_gracza )
					//	continue;
					collisionY = false;
					collisionX = false;
					if( moveX_start + moveX_change + playerW/2.0f > (*object_it)->getX()
						&& (*object_it)->getX() + (*object_it)->getWidth() > moveX_start + moveX_change - playerW/2.0f )
						collisionX = true;
					if( moveY_start + moveY_change + playerH/2.0f > (*object_it)->getY()
						&& (*object_it)->getY() + (*object_it)->getHeight() > moveY_start + moveY_change - playerH/2.0f )
						collisionY = true;
					if( collisionX && collisionY )
						collision = true;
					//* Gracz stoi na przycisku?
					Button *button = dynamic_cast<Button*>(obstacle);
					if( button == obstacle && !button->isBlocked() ) {
						if( moveX_start + moveX_change + playerW/2.0f > button->getX() + 0.1f
						&& button->getX() + button->getWidth() > moveX_start + moveX_change - playerW/2.0f - 0.1f
						&& moveY_start + moveY_change + playerH/2.0f > button->getY() + 0.1f
						&& button->getY() + button->getHeight() > moveY_start + moveY_change - playerH/2.0f - 0.1f )
							button->activate();
						else
							button->deactivate();
						//collision = false;
					}//*/
				}//*/
				if( !collision ) {
					moveX = moveX_start + moveX_change;
					moveY = moveY_start + moveY_change;
				}/*
				glPushMatrix();
				glTranslatef( moveX + moveX_value * 2, moveY + moveY_value * 2, 0 );
				glutSolidCube( 0.1f );
				glPopMatrix();*/
				// Je¿eli niesiemy kostkê, to rysuj j¹ przed graczem i sprawdŸ czy nie ma na niej przycisku
				if( carrying ) {
					moveCarryingItem();
				}
				// Sprawdzanie, czy kostka, b¹dŸ gracz nie znajduje siê na przycisku.
				//if(  ) ;
				if( collision || now > moveF_time_end ) {
					state_list.remove( STATE_MOVE_FORWARD );
					state_list.remove( STATE_MOVE_BACKWARD );
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
				break;

			case STATE_FIRE:
				fire( now );
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
				if( carrying )
					moveCarryingItem();
				if ( now > rotateY_time_end )
					state_list.remove( STATE_CHANGE_ROTATION_RIGHT );
				break;
				
			case STATE_CHANGE_ROTATION_LEFT:
				rotateY = rotateY_start + rotateY_value * ( now - rotateY_time_start ) / ( rotateY_time_end - rotateY_time_start );
				if( rotateY >= 360.0f )
					rotateY = 0.0f;
				if( carrying )
					moveCarryingItem();
				if ( now > rotateY_time_end )
					state_list.remove( STATE_CHANGE_ROTATION_LEFT );
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

void CGL::moveCarryingItem()
{
	carryX = moveX + cos( ( M_PI * rotateY ) / 180.0f + M_PI/2 ) * 0.3f * 2.7f - closeItem->getWidth()/2.0f;
	carryY = moveY + sin( ( M_PI * rotateY ) / 180.0f + M_PI/2 ) * 0.3f * 2.7f - closeItem->getHeight()/2.0f;
	closeItem->moveTo( carryX, carryY );
	closeItem->rotate( rotateY );
	// Je¿eli kosta znajduje siê na przycisku, to aktywuj go. W przeciwnym wypadku dezaktywuj.
	list<Obstacle*> objects_aux = objects_list;
	list<Obstacle*>::iterator object_aux_it;
	for( object_aux_it = objects_aux.begin(); object_aux_it != objects_aux.end(); object_aux_it++ ) {
		obstacle = *object_aux_it;
		if( Button *button = dynamic_cast<Button*>(obstacle) ) {
			// Je¿eli kostka nie jest ko³o przycisku
			if( sqrtf( powf( moveX - button->getX() - button->getWidth()/2.0f, 2.0f )
					+ powf( moveY - button->getY() - button->getHeight()/2.0f, 2.0f ) ) > 1.0f )
					continue;
			// Cos tu brzydko z lapaniem kostki jest!!!!!!!!!!!!!!!!!!!!!!
			if( closeItem->getX() > button->getX() - 0.3f
			&& button->getX() + button->getWidth() > closeItem->getX() - closeItem->getWidth() + 0.3f
			&& closeItem->getY() > button->getY() - 0.3f
			&& button->getY() + button->getHeight() > closeItem->getY() - closeItem->getHeight() + 0.3f ) {
				button->activate();
				button->block();
			} else {
				button->unblock();
				button->deactivate();
			} 
		}
	}
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
					(float)y / (float)board_h - 0.5f,
					0.0f );
				glScalef(
					1.0f / (float)board_w,
					1.0f / (float)board_h,
					0.4f );
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
	for( object_it = objects.begin(); object_it != objects.end(); object_it++ )
		(*object_it)->draw();
}

void CGL::fire( float time ) {
	
	//

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
		carry_time_start = getTime();
		if( carry_time_start < carry_time_end )
			return;

		if( menu )
			exit(0);
		else {
			menu = true;
			state_list.push_front( MENU );
			state_list.remove( MOVE_CAMERA );
		}
		
		carry_time_end = carry_time_start + 200;
	}

	if( keyStates[13] ) {
		carry_time_start = getTime();
		if( carry_time_start < carry_time_end )
			return;
		if( menu ) {
			state_list.push_front( MOVE_CAMERA );
			state_list.remove( MENU );
			menu = false;
		}
		
		carry_time_end = carry_time_start + 200;
	}
	
	if ( keyStates['q'] ) {
		if (!findState(STATE_CHANGE_ROTATION_LEFT)) {
			state_list.remove( STATE_CHANGE_ROTATION_RIGHT );
			state_list.push_front( STATE_CHANGE_ROTATION_LEFT );
			rotateY_start = rotateY;
			rotateY_value = 30;
			rotateY_time_start = getTime();
			rotateY_time_end = rotateY_time_start + 100;
		}
	}	
	
	if ( keyStates['e'] ) {
		if (!findState(STATE_CHANGE_ROTATION_RIGHT)) {
			state_list.remove( STATE_CHANGE_ROTATION_LEFT );
			state_list.push_front( STATE_CHANGE_ROTATION_RIGHT );
			rotateY_start = rotateY;
			rotateY_value = 30;
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

	if( keyStates[' '] ) {
		if( !findState( STATE_FIRE ) ) {
			state_list.push_front( STATE_FIRE );
		}
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
		}
	}

	// Sprawdza czy w pewnym promieniu gracza nie ma jakiegoœ obiektu, który móg³by podnieœæ.
	// Je¿li jest, to ustaw closeItem na przedmiot i go podnieœ.
	if( keyStates['f'] ) {
		carry_time_start = getTime();
		if( carry_time_start < carry_time_end )
			return;
		// Je¿eli kostka nie jest trzymana i znajduje siê w odleg³oœci mnieszej ni¿ 0.5 od bohatera,
		// to z³ap. Je¿eli kostka jest trzymana i naciœnie sie klawisz to puœæ nie sprawdzaj¹c nic.
		/*
		if( !carrying )
			for( dla ka¿ej przeszkody, któr¹ mo¿na nosiæ )
				if( przeszkoda nale¿y do pewnego promienia gracza
					&& przedmiot jest przed graczem ) {
					closeItem = przeszkoda;
					carrying = true;
					break; // Bo nie mo¿e nosiæ dwóch przeszkód na raz
				}
		else
			carrying = false;
		/*/	
		if( carrying ) {
			closeItem = NULL;
			carrying = false;
		} else {
			list<Obstacle*> objects = carryableItems_list;
			list<Obstacle*>::iterator object_it;
			for( object_it = objects.begin(); object_it != objects.end(); object_it++ )
				// Je¿eli kostka znajduje siê w promieniu gracza.
				// Brakuje sprawdzania, czy obiekt jest przed graczem.
				if( sqrtf( powf( moveX - (*object_it)->getX() - (*object_it)->getWidth()/2.0f, 2.0f )
					+ powf( moveY - (*object_it)->getY() - (*object_it)->getHeight()/2.0f, 2.0f ) ) < 1
					&& true /* gracz patrzy na kostkê */ ) {
						cout << "kat( gracz, obiekt ): "
							<< ( M_PI - atan2f( moveY - (*object_it)->getY(), moveX - (*object_it)->getX() ) ) * 180 / M_PI;
						cout << " rotateY: " << rotateY << endl;
					closeItem = *object_it;
					carrying = true;
					// Zachowaj aktualny k¹t obrotu i dodaj zmieniany
					//closeItem->rotationStart( rotateY );
					closeItem->draw();
					break;
				}
		}
		/*/
		if( carrying || !carrying
			&& abs( moveX - cube->getX() - cube->getWidth()/2.0f ) < 1.0f
			&& abs( moveY - cube->getY() - cube->getHeight()/2.0f ) < 1.0f )
			carrying = !carrying; //*/
		/*
		if( !findState( STATE_CARRYING_CUBE ) ) {
			state_list.push_front( STATE_CARRYING_CUBE );
		} else {
			state_list.remove( STATE_CARRYING_CUBE );
		}//*/
		carry_time_end = carry_time_start + 200;
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
char** CGL::read_board( void ) {

	FILE * input;
	input = fopen( "board.txt", "rt" );
	
	if( input == NULL )
		return NULL;
	
	//int board_w, board_h;
	
	fscanf( input, "%d,%d\n", &board_w, &board_h );	
	printf( "board size: %d,%d\n", board_w, board_h );

	char symbol;
	
	char** board = new char*[board_h];
	
	// Pocz¹tek planszy w pliku jest u góry po lewej, zaœ w grze na dole po lewej.
	// Temu dla rysowania czytamy y od ty³u.
	for( int y = 0; y < board_h; y++ ) {
	//for( int y = board_h-1; y; y-- ) { 
		board[-y+board_h-1] = new char[board_w];
		
		for( int x = 0; x < board_w; x++ ) {
			if( fscanf( input, "%c", &symbol ) == EOF )
				return NULL;
			
			board[-y+board_h-1][x] = symbol;
		}
		fscanf( input, "%c", &symbol );
	}

	//fscanf( input, "%d\n", &connections )
	//while( connections-- && !feof( input ) )
	//	fscanf( input, "%c:%s\n", &callee, &caller );
	//	callees += callee;
	//	callers += caller;
	//parse_connections( callees, callers );

	fclose( input );
	
	return board;
}

void CGL::create_obstacle( float x, float y, float width, float height ) {
	Obstacle * object = new Obstacle( x, y, 0.0f, 1.0f, width, height );
	cgl.objects_list.push_back( object );
}

void CGL::board_to_obstacles( char ** board )
{
	Obstacle * object, object2;
	ExitDoor * exitDoor, *exitDoor2;

	float gx, gy;
	float block_size = 1.0;
	
	float buttonX, buttonY;
	float button2X, button2Y;

	for( int y = 0; y < board_h; y++ )
		for( int x = 0; x < board_w; x++ ) {
			gx = x * block_size - range * block_size - block_size;
			gy = y * block_size - range * block_size - block_size;
			switch( board[y][x] )
			{
			case ' ':
				break;
			case '#':
				object = new Obstacle( gx, gy, 0.0f, block_size, block_size, block_size );
				cgl.objects_list.push_back( object );
				break;
			case '^': // Start
				moveX = gx;
				moveY = gy;
				break;
			case '$': // Koniec
				exitDoor = new ExitDoor( gx, gy, 0.0f );
				cgl.objects_list.push_back( exitDoor );
				break;
			case '&':
				exitDoor2 = new ExitDoor( gx, gy, 0.0f );
				cgl.objects_list.push_back( exitDoor2 );
				break;
			case '@': // Kostka
				object = new Cube( gx, gy, 0.0f );
				objects_list.push_front( object );
				carryableItems_list.push_front( object );
			case '%': // Wie¿yczka :x
				break;
			case 'a': // Przycisk
				// Je¿eli drzwi bêda wczeœniej w plansz ni¿ przycisk, to wskaŸnik exit nie bêdzie mia³ poprawnej waroœci.
				buttonX = gx;
				buttonY = gy;
				break;
			case 'b':
				button2X = gx;
				button2Y = gy;
				break;
			default:
				// Litera to przycisk.
				/*/ Kostka na przycisku otwiera drzwi.
				char symbol = board[y][x];
				if( 'a' >= symbol && symbol >= 'z' )
					//
					break;*/
				cout << "'" << board[y][x] << "'? Nie ogarniam. D:" << endl;
			}
		}
		// Przycisk inicjalizujemy po petli, ale trzba bedzie ladniej to napisac.
		object = new Button( buttonX, buttonY, -0.01f, exitDoor );
		objects_list.push_back( object );
		object = new Button( button2X, button2Y, -0.01, exitDoor2 );
		objects_list.push_back( object );
}

/*
void info( char * text, int x, int y ) {
	glMaterialfv( GL_FRONT, GL_DIFFUSE, colorDim );
	glMaterialfv( GL_FRONT, GL_AMBIENT, colorDim );
	glMaterialfv( GL_FRONT, GL_EMISSION, colorGray );
	glMaterialfv( GL_FRONT, GL_SPECULAR, colorDim );
	CUTIL::renderBitmapString( x, y, GLUT_BITMAP_HELVETICA_18, text );
}
*/
