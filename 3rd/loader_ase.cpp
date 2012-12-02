#include "loader_ase.h"

ASE::ASE(string filepath) {
	GEOMOBJECT_COUNT = 0;
	SUBMATERIAL_COUNT = 0;

	ifstream file(filepath.c_str());
	if(!file.is_open()) exit(0);

	char line_buffer[256];
	string tmp_str;

	int submaterial = -1;
	int material = -1;
	int level = 0;

	while(file.getline(line_buffer, 256, '\n')) {
		istringstream stream(line_buffer);

		string str_line = stream.str();
		size_t found = str_line.find_first_of( "{" );
		if( found != string::npos ) {
			level++;
		}

		stream >> tmp_str;

		if( tmp_str == "}" ) {
			level--;
		}

		else if( tmp_str == "*SUBMATERIAL" && level == 3 ) {
			stream >> submaterial;
			SUBMATERIAL_COUNT++;
		}

		else if( tmp_str == "*MATERIAL_AMBIENT" && level == 3 ) {
			if( submaterial >= 0 ) {
				stream >> SUBMATERIAL[submaterial].ambient[0];
				stream >> SUBMATERIAL[submaterial].ambient[1];
				stream >> SUBMATERIAL[submaterial].ambient[2];
			}
		}
		else if( tmp_str == "*MATERIAL_DIFFUSE" && level == 3 ) {
			if( submaterial >= 0 ) {
				stream >> SUBMATERIAL[submaterial].diffuse[0];
				stream >> SUBMATERIAL[submaterial].diffuse[1];
				stream >> SUBMATERIAL[submaterial].diffuse[2];
			}
		}
		else if( tmp_str == "*MATERIAL_SPECULAR" && level == 3 ) {
			if( submaterial >= 0 ) {
				stream >> SUBMATERIAL[submaterial].specular[0];
				stream >> SUBMATERIAL[submaterial].specular[1];
				stream >> SUBMATERIAL[submaterial].specular[2];
			}
		}
		else if( tmp_str == "*MATERIAL_SHINESTRENGTH" && level == 3 ) {
			if( submaterial >= 0 ) {
				stream >> SUBMATERIAL[submaterial].shine;
				SUBMATERIAL[submaterial].shine *= 100.0f;
			}
		}


		else if( tmp_str == "*MATERIAL" && level == 2 ) {
			stream >> submaterial;
			SUBMATERIAL_COUNT++;
		}
		else if( tmp_str == "*MATERIAL_AMBIENT" && level == 2 ) {
			if( submaterial >= 0 ) {
				stream >> SUBMATERIAL[submaterial].ambient[0];
				stream >> SUBMATERIAL[submaterial].ambient[1];
				stream >> SUBMATERIAL[submaterial].ambient[2];
			}
		}
		else if( tmp_str == "*MATERIAL_DIFFUSE" && level == 2 ) {
			if( submaterial >= 0 ) {
				stream >> SUBMATERIAL[submaterial].diffuse[0];
				stream >> SUBMATERIAL[submaterial].diffuse[1];
				stream >> SUBMATERIAL[submaterial].diffuse[2];
			}
		}
		else if( tmp_str == "*MATERIAL_SPECULAR" && level == 2 ) {
			if( submaterial >= 0 ) {
				stream >> SUBMATERIAL[submaterial].specular[0];
				stream >> SUBMATERIAL[submaterial].specular[1];
				stream >> SUBMATERIAL[submaterial].specular[2];
			}
		}
		else if( tmp_str == "*MATERIAL_SHINESTRENGTH" && level == 2 ) {
			if( submaterial >= 0 ) {
				stream >> SUBMATERIAL[submaterial].shine;
				SUBMATERIAL[submaterial].shine *= 100.0f;
			}
		}


		else if( tmp_str == "*GEOMOBJECT" ) 
			GEOMOBJECT_COUNT++;


		else if(tmp_str == "*NODE_NAME" && GEOMOBJECT[GEOMOBJECT_COUNT-1].NODE_NAME.size() == 0) {
			stream >> tmp_str;
			tmp_str = tmp_str.substr(1,tmp_str.length()-2);
			size_t s = tmp_str.find(".");
			if(s != string::npos) tmp_str = tmp_str.substr(0, s);
			GEOMOBJECT[GEOMOBJECT_COUNT-1].NODE_NAME = tmp_str;
		}
		else if(tmp_str == "*MESH_NUMVERTEX") {
			stream >> GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_NUMVERTEX;
			GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_VERTEX = new float*[GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_NUMVERTEX];
		}
		else if(tmp_str == "*MESH_NUMFACES") {
			stream >> GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_NUMFACES;
			GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_FACE = new int*[GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_NUMFACES];
			GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_FACENORMAL = new float*[GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_NUMFACES];
		}
		else if(tmp_str == "*MESH_VERTEX") {
			int id;
			stream >> id;
			GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_VERTEX[id] = new float[3];
			stream >> GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_VERTEX[id][0];
			stream >> GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_VERTEX[id][1];
			stream >> GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_VERTEX[id][2];
		}
		else if(tmp_str == "*MESH_FACE") {
			int id;
			stream >> id;
			GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_FACE[id] = new int[4];
			stream >> tmp_str;
			stream >> tmp_str;
			stream >> GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_FACE[id][0];
			stream >> tmp_str;
			stream >> GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_FACE[id][1];
			stream >> tmp_str;
			stream >> GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_FACE[id][2];

			while( !stream.eof() ){
				stream >> tmp_str;	
				if( tmp_str == "*MESH_MTLID" ) {
					stream >> GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_FACE[id][3];
				}

			};


		}
		else if(tmp_str == "*MESH_NUMTVERTEX") {
			stream >> GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_NUMTVERTEX;
			GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_TVERT = new float*[GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_NUMTVERTEX];
		}
		else if(tmp_str == "*MESH_TVERT") {
			int id;
			stream >> id;
			GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_TVERT[id] = new float[2];
			stream >> GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_TVERT[id][0];
			stream >> GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_TVERT[id][1];
		}
		else if(tmp_str == "*MESH_NUMTVFACES") {
			stream >> GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_NUMTVFACES;
			GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_TFACE = new int*[GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_NUMTVFACES];
		}
		else if(tmp_str == "*MESH_TFACE") {
			int id;
			stream >> id;
			GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_TFACE[id] = new int[3];
			stream >> GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_TFACE[id][0];
			stream >> GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_TFACE[id][1];
			stream >> GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_TFACE[id][2];
		}
		else if(tmp_str == "*MESH_FACENORMAL") {
			int id;
			stream >> id;
			GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_FACENORMAL[id] = new float[3];
			stream >> GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_FACENORMAL[id][0];
			stream >> GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_FACENORMAL[id][1];
			stream >> GEOMOBJECT[GEOMOBJECT_COUNT-1].MESH_FACENORMAL[id][2];
		}
	}
	file.close();
}