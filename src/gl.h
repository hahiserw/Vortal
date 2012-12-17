#ifndef __GL_H
#define __GL_H

#include <iostream>
#include <list>

using namespace std;


/** Glowna klasa odpowiedzialna za sterowanie aplikacja i rysowanie grafiki. Klasa jest 'wrapperem' dla funkcji wywolywanych 
bezposrednio przez API OpenGL'a. Jej zastosowanie ma na celu wykorzystanie mechanizmow obiektowych do programowania OpenGL'a
(natywnie API OpenGL'a wymaga programowania w ANSI C)). W klasie znajduje sie szereg funkcji wywolywanych przez maszyne 
stanu OpenGL'a. Przykladem moze byc funkcja display() wywolywana w momencie, gdy wymagane jest przerysowanie ekranu.
*/


static float M_PI = 3.14159265358979323846;

static float colorDim[] = {0.01f,0.01f,0.01f};
static float colorWhite[] = {0.9f,0.9f,0.9f};
static float colorGreen[] = {0.01f,0.7f,0.01f};
static float colorGray[] = {0.4f,0.4f,0.4f};
static float colorBlack[] = { 0.0f, 0.0f, 0.0f };

class Obstacle;
class Cube;
class Shoot;

class CGL {

public: 
	static const int TIMER = 10; // Wartoœæ skoku dla maszyny stanów

	list<int> state_list;
	list<Obstacle*> objects_list;
	list<Shoot*> shoots_list;

	Obstacle * cube;

	int board_w;
	int board_h;
	float map_w, map_h;

	/**< Animacja szescianu */
	float rotateY;
	float rotateY_start;
	float rotateY_value;
	float rotateY_time_start;
	float rotateY_time_end;

	float moveF_time_start;
	float moveF_time_end;
	float moveS_time_start;
	float moveS_time_end;
	
	float moveY_start, moveY_value, moveY_change;
	float moveX_start, moveX_value, moveX_change;
	float range;

	float carry_time_start;
	float carry_time_end;
	
	/** Obs³uga myszy */
	float translateX;
	float translateY;
	int mouse_button;
	int mouse_x;
	int mouse_y;
	float mouse_move_x;
	float mouse_move_y;
	float cameraX;
	
	bool flag_specular;
	bool flag_diffuse;
	bool flag_phong;

	//@ Dodajemy sobie zmienne odpowiadaj¹ce za animacje
	float headX;
	bool changeX;
	float moveX, moveY;
	float playerW, playerH;
	bool move;
	bool collision;
	bool collisionX;
	bool collisionY;


	GLuint shaderprogram;

	int** board;
	
	bool* keyStates; // bufor klawiatury

	
private:
	int fps; /**< liczba klatek generowanych na sekunde */

	GLuint texture; /**< Indeks tekstury. */

	GLuint aseGeometry; /**< Uchwyt do obiektu ASE. */

public:

	CGL() {
		
		state_list.clear();
		objects_list.clear();
		shoots_list.clear();

		rotateY = 0.0f;
		texture = 1;
		mouse_button = 0;
		mouse_move_x = 0;
		mouse_move_y = 0;
		translateX = 0.0f;
		translateY = 0.0f;
		board = NULL;
		flag_specular = true;
		flag_diffuse = true;
		flag_phong = false;
		cameraX = 0.0f;

		//@ Inicjujemy nasze zmienne :D
		headX = 0;
		changeX = false;
		moveX = 0;
		moveY = 0;
		playerW = 0.5;
		playerH = 0.5;
		range = 5;
		move = false;
		collision = false;
		collisionX = false;
		collisionY = false;
		
		keyStates = new bool[256]; // Stany dla wszyskich klawiszy
		for(int i=0;i<256;i++)
			keyStates[i] = 0;
		
		
		
	}


	/** Inicjalizacja srodowiska graficznego: wczytanie tekstury, ustawienie kamery i swiatel, wczytanie obiektu ASE. Inicjalizacja
		odbywa sie tylko raz po uruchomieniu programu.
	*/
	void init( void );
	
	/** Glowna procedura rysowania grafiki. Wywolywana jest wtedy, gdy ekran wymaga przerysowania (np. obiekt zmienia swoje polozenie, okno bylo zasloniete przez inna aplikacje, itp.).  
	*/
	void display( void );
	
	/** Rysowanie obiektu
	*/
	void draw_clown( float headX );
	void move_head( void );
	void draw_cube( void );
	void draw_cube( float angle );
	void draw_teapot( float angle );
	void draw_cube_texture( float angle );
	void create_obstacle( float, float, float, float );
	void draw_obstacles( /*list<Obstacle*> &, list<Obstacle*>::iterator &*/ );
	void board_to_obstacles( int ** );
	void fire( float );

	void info( char * );

	/** Odczytanie danych planszy gry z pliku 
	 */
	int** read_board( void );
	
	/** Rysowanie planszy gry
	 */
	void draw_board( void );


	/** Funkcja wywolywana w przypadku braku aktywnosci innych elementow programu (obecnie funkcja nie jest wykorzystywana). 
	*/
	void idle( void );
	
	/** Funkcja wywolywana cyklicznie co pewien interwal czasu (zobacz TIMER). Odbywa sie w niej zmiana polozenia gracza i przeciwnika oraz zwiekszany jest licznik czasu. 
	*/
	void timer( void );
	

	/** Funkcja obslugujaca klawiature. Zmiana kierunku ruchu gracza nastepuje po nacisnieciu klawiszy '[' oraz ']'. Klawisz 'Esc' przerywa gre.
	*/
	void keyboardPress( unsigned char key,int a,int b );
	void keyboardUp( unsigned char key,int a,int b );
	void keyOperations( void );
	
	/** Obsluga ruchy myszy. Zmiana polozenia kursora myszy z jednoczesnym wcisnieciem lewego przycisku myszy powoduje obrot planszy.
	*/
	void mouseMotion( int x, int y );
	
	/** Obsluga klawiszy myszy, funkcja wywolywana jest po nacisnieciu jednego z klawiszy myszy. Wspomaganie obracania planszy.
	*/
	void mousePress( int button, int state, int x, int y );

	/** Czas systemowy w milisekundach 
	 */
	float getTime( void );
	

	/**
	 */
	bool findState( int state ) {
		
		list<int> states = state_list;
		list<int>::iterator state_it;
		
		for( state_it = states.begin(); state_it != states.end(); ++state_it) {
			
			if ( *state_it == state ) {
				return true;
			}
		}
		return false;
	}
	
	
};

extern CGL cgl;

#endif


