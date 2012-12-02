#ifndef __UTIL_H
#define __UTIL_H

#ifndef MAC
#include <windows.h>
#endif

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <ios>

#include "../3rd/loader_ase.h"

#pragma warning(disable:4996)
using namespace std;

/** Klasa z pomocniczymi narzedziami (np. funkcja wczytujaca bitmape z pliku dyskowego). 
*/
class CUTIL {
public:

	
#ifndef MAC
	/** Wczytywanie bitmapy z pliku dyskowego, bitmapa moze byc wykorzystywana jako tekstura nakladana na obiekty.
	@return Uchwyt do tektury.
	*/
	static unsigned char* LoadBitmapFile( char *filename, BITMAPINFOHEADER *bitmapInfoHeader ) {

		FILE *filePtr;
		BITMAPFILEHEADER bitmapFileHeader;
		unsigned char *bitmapImage;
		int	imageIdx = 0;
		unsigned char tempRGB;

		filePtr = fopen(filename, "rb");
		if (filePtr == NULL)
			return NULL;

		fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	
		fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

		fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

		bitmapImage = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage);

		if (!bitmapImage) {
			free(bitmapImage);
			fclose(filePtr);
			return NULL;
		}

		fread(bitmapImage, 1, bitmapInfoHeader->biSizeImage, filePtr);

		if (bitmapImage == NULL) {
			fclose(filePtr);
			return NULL;
		}

		for (imageIdx = 0; imageIdx < (int)bitmapInfoHeader->biSizeImage; imageIdx+=3) {
			tempRGB = bitmapImage[imageIdx];
			bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
			bitmapImage[imageIdx + 2] = tempRGB;
		}
		fclose(filePtr);

		return bitmapImage;
	}
#endif

	/** Wczytanie obiektu w formacie ASE z pliku dyskowego. Funkcja wczytuje wierzcholki, wektory normalen i parametry materialow. 
	Nie sa wczytywane wspolrzedne tektury.
	@return Uchwyt do obiektu.
	*/
	static GLuint loadASE( string filename ) {

		ASE *data = new ASE( filename );

		int cc_objs = data->GEOMOBJECT_COUNT;
		cerr << "no of objects: " << cc_objs << endl;
		int cc_mats = data->SUBMATERIAL_COUNT;
		cerr << "no of materials: " << cc_mats << endl;

		int list = glGenLists (2);
		glNewList( list, GL_COMPILE);
		
		for( int i = 0; i < cc_objs; i++ ) {

			glBegin(GL_TRIANGLES);

			int cc_faces = data->GEOMOBJECT[i].MESH_NUMFACES;
		
			cerr << "no of faces: " << cc_faces << endl;

			float col[4];
			col[3] = 1.0f;
			int idx_mat;

			for( int j = 0; j < cc_faces; j++ ) {

				// material
				idx_mat = data->GEOMOBJECT[i].MESH_FACE[j][3];			

				col[0] = data->SUBMATERIAL[idx_mat].diffuse[0];
				col[1] = data->SUBMATERIAL[idx_mat].diffuse[1];
				col[2] = data->SUBMATERIAL[idx_mat].diffuse[2];
				glMaterialfv( GL_FRONT, GL_DIFFUSE, col );

				col[0] /= 10;
				col[1] /= 10;
				col[2] /= 10;
				//col[0] = col[1] = 0.3;
				glMaterialfv( GL_FRONT, GL_EMISSION, col );
				
				col[0] = data->SUBMATERIAL[idx_mat].ambient[0];
				col[1] = data->SUBMATERIAL[idx_mat].ambient[1];
				col[2] = data->SUBMATERIAL[idx_mat].ambient[2];
				glMaterialfv( GL_FRONT, GL_AMBIENT, col );
				
				col[0] = data->SUBMATERIAL[idx_mat].specular[0];
				col[1] = data->SUBMATERIAL[idx_mat].specular[1];
				col[2] = data->SUBMATERIAL[idx_mat].specular[2];
				glMaterialfv( GL_FRONT, GL_SPECULAR, col );
				
				glMaterialf( GL_FRONT, GL_SHININESS, data->SUBMATERIAL[idx_mat].shine );

				for(int k = 0; k < 3; k++) {

					if( data->GEOMOBJECT[data->GEOMOBJECT_COUNT-1].MESH_NUMTVERTEX > 0 ) {
						float tex_u = data->GEOMOBJECT[i].MESH_TVERT[data->GEOMOBJECT[i].MESH_TFACE[j][k]][0];
						float tex_v = data->GEOMOBJECT[i].MESH_TVERT[data->GEOMOBJECT[i].MESH_TFACE[j][k]][1];
						glTexCoord2f( tex_u, tex_v );
					}
					
					float normal_x = data->GEOMOBJECT[i].MESH_FACENORMAL[j][0];
					float normal_y = data->GEOMOBJECT[i].MESH_FACENORMAL[j][2];
					float normal_z = data->GEOMOBJECT[i].MESH_FACENORMAL[j][1];
					glNormal3f( normal_x, normal_y, normal_z );

					float vert_x = data->GEOMOBJECT[i].MESH_VERTEX[data->GEOMOBJECT[i].MESH_FACE[j][k]][0];
					float vert_y = data->GEOMOBJECT[i].MESH_VERTEX[data->GEOMOBJECT[i].MESH_FACE[j][k]][2];
					float vert_z = data->GEOMOBJECT[i].MESH_VERTEX[data->GEOMOBJECT[i].MESH_FACE[j][k]][1];
					glVertex3f( vert_x, vert_y, vert_z );

				}

			}

			glEnd();
			glEndList();
		}

		delete data;

		return list;
	}

	/** Funkcja rysuje szescian (podobnie jak glutSolidCube(), ale definiuje rowniez wektory normalne i koordynaty tekstury).
	*/
	static void drawBox( void ) {
		
		
		glBegin(GL_QUADS);		// Draw The Cube Using quads
		 
		glNormal3f( 0, 1.0f, 0 );
		glTexCoord2f( 0.0f, 0.0f );
		glVertex3f( 1.0f, 1.0f,-1.0f);	// Top Right Of The Quad (Top)
		glTexCoord2f( 0.0f, 1.0f );
		glVertex3f(-1.0f, 1.0f,-1.0f);	// Top Left Of The Quad (Top)
		glTexCoord2f( 0.0f, 0.0f );
		glVertex3f(-1.0f, 1.0f, 1.0f);	// Bottom Left Of The Quad (Top)
		glTexCoord2f( 1.0f, 0.0f );
		glVertex3f( 1.0f, 1.0f, 1.0f);	// Bottom Right Of The Quad (Top)
		 
		glNormal3f( 0, -1.0f, 0 );
		glTexCoord2f( 0.0f, 0.0f );
		glVertex3f( 1.0f,-1.0f, 1.0f);	// Top Right Of The Quad (Bottom)
		glTexCoord2f( 0.0f, 1.0f );
		glVertex3f(-1.0f,-1.0f, 1.0f);	// Top Left Of The Quad (Bottom)
		glTexCoord2f( 0.0f, 0.0f );
		glVertex3f(-1.0f,-1.0f,-1.0f);	// Bottom Left Of The Quad (Bottom)
		glTexCoord2f( 1.0f, 0.0f );
		glVertex3f( 1.0f,-1.0f,-1.0f);	// Bottom Right Of The Quad (Bottom)

		glNormal3f( 0, 0, 1.0f );
		glTexCoord2f( 0.0f, 0.0f );
		glVertex3f( 1.0f, 1.0f, 1.0f);	// Top Right Of The Quad (Front)
		glTexCoord2f( 0.0f, 1.0f );
		glVertex3f(-1.0f, 1.0f, 1.0f);	// Top Left Of The Quad (Front)
		glTexCoord2f( 0.0f, 0.0f );
		glVertex3f(-1.0f,-1.0f, 1.0f);	// Bottom Left Of The Quad (Front)
		glTexCoord2f( 1.0f, 0.0f );
		glVertex3f( 1.0f,-1.0f, 1.0f);	// Bottom Right Of The Quad (Front)

		glNormal3f( 1.0f, 0, 0.0f );
		glTexCoord2f( 0.0f, 0.0f );
		glVertex3f( 1.0f, 1.0f,-1.0f);	// Top Right Of The Quad (Right)
		glTexCoord2f( 0.0f, 1.0f );
		glVertex3f( 1.0f, 1.0f, 1.0f);	// Top Left Of The Quad (Right)
		glTexCoord2f( 0.0f, 0.0f );
		glVertex3f( 1.0f,-1.0f, 1.0f);	// Bottom Left Of The Quad (Right)
		glTexCoord2f( 1.0f, 0.0f );
		glVertex3f( 1.0f,-1.0f,-1.0f);	// Bottom Right Of The Quad (Right)
		 
		glNormal3f( -1.0f, 0, 0.0f );
		glTexCoord2f( 0.0f, 0.0f );
		glVertex3f(-1.0f, 1.0f, 1.0f);	// Top Right Of The Quad (Left)
		glTexCoord2f( 0.0f, 1.0f );
		glVertex3f(-1.0f, 1.0f,-1.0f);	// Top Left Of The Quad (Left)
		glTexCoord2f( 0.0f, 0.0f );
		glVertex3f(-1.0f,-1.0f,-1.0f);	// Bottom Left Of The Quad (Left)
		glTexCoord2f( 1.0f, 0.0f );
		glVertex3f(-1.0f,-1.0f, 1.0f);	// Bottom Right Of The Quad (Left)
		
		
		glNormal3f( 0, 0, -1.0f );
		glTexCoord2f( 0.0f, 0.0f );
		glVertex3f( 1.0f,-1.0f,-1.0f);	// Top Right Of The Quad (Back)
		glTexCoord2f( 0.0f, 1.0f );
		glVertex3f(-1.0f,-1.0f,-1.0f);	// Top Left Of The Quad (Back)
		glTexCoord2f( 0.0f, 0.0f );
		glVertex3f(-1.0f, 1.0f,-1.0f);	// Bottom Left Of The Quad (Back)
		glTexCoord2f( 1.0f, 0.0f );
		glVertex3f( 1.0f, 1.0f,-1.0f);	// Bottom Right Of The Quad (Back)	
		
		glEnd();			// End Drawing The Cube		


}


	/** Rysowanie tekstu na ekranie.
	@param x - wspolrzedna x tekstu
	@param y - wspolrzedna y tekstu
	@param z - wspolrzedna z tekstu
	@param font - rodzaj czcionki
	@param string - tekst, ktory ma byc narysowany 
	*/
	static void renderBitmapString( int x, int y, void *font, char *string) {  
		char *c;
		glRasterPos2i( x, y );
		for (c=string; *c != '\0'; c++) {
			glutBitmapCharacter(font, *c);
		}
	}

	/** Wyswietlanie komunikatu w konsoli
	*/
	static void show_message( char* msg ) {
		printf( "%s\n", msg );
	}

	
	/* A simple function that will read a file into an allocated char pointer buffer */
	static char* filetobuf(char *file)
	{
		FILE *fptr;
		long length;
		char *buf;
		
		fptr = fopen(file, "rb"); /* Open file for reading */
		if (!fptr){ /* Return NULL on failure */
			printf("Cannot load input file (%s)\n", file);
			return NULL;
		}
		fseek(fptr, 0, SEEK_END); /* Seek to the end of the file */
		length = ftell(fptr); /* Find out how many bytes into the file we are */
		buf = (char*)malloc(length+1); /* Allocate a buffer for the entire length of the file and a null terminator */
		fseek(fptr, 0, SEEK_SET); /* Go back to the beginning of the file */
		fread(buf, length, 1, fptr); /* Read the contents of the file in to the buffer */
		fclose(fptr); /* Close the file */
		buf[length] = 0; /* Null terminator */
		
		return buf; /* Return the buffer */
	}
	
	
	
};





#endif



