#ifndef __LOADER_ASE_H__
#define __LOADER_ASE_H__

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

using namespace std;

struct ASE_GEOMOBJECT {
	string NODE_NAME;
	int MESH_NUMVERTEX;
	float **MESH_VERTEX;
	int MESH_NUMFACES;
	int **MESH_FACE;
	int MESH_NUMTVERTEX;
	float **MESH_TVERT;
	int MESH_NUMTVFACES;
	int **MESH_TFACE;
	float **MESH_FACENORMAL;
};

class ASE_SUBMATERIAL {
public:
	float ambient[3];
	float diffuse[3];
	float specular[3];
	float shine;
};


class ASE {
public:
	int GEOMOBJECT_COUNT;
	map<int, ASE_GEOMOBJECT> GEOMOBJECT;
	ASE(string filepath);

	int SUBMATERIAL_COUNT;
	map<int, ASE_SUBMATERIAL> SUBMATERIAL;

};

#endif //__LOADER_ASE_H__