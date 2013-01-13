/*
 * Cube rozszerza Obstacle
 * Posiada dodatkowe wlasciwosci
 */

class Cube: public Obstacle
{
	float globalRotation;
public:
	Cube( float, float, float );
	void rotationStart( float );
	void draw() const;
};


Cube::Cube( float x, float y, float z )
{
	this->x = x;
	this->y = y;
	this->z = z;
	length = 0.6;
	width = 0.6;
	height = 0.6;
	rotation = 0;
	globalRotation = 0;
	//Obstacle::Obstacle( x, y, z, 0.6f, 0.6f, 0.6f );
}

void Cube::rotationStart( float rotation )
{
	globalRotation = rotation;
}

void Cube::draw() const
{
	glPushMatrix();
	glTranslatef( x + length/2.0f, y + width/2.0f, 0.0f ); // Bo skalowanie jest wzgledem srodka
	//glTranslatef( (float)-width/2.0f, (float)-height/2.0f, 0.0f );
	glRotatef( globalRotation, 0.0f, 0.0f, 1.0f );
	glRotatef( rotation, 0.0f, 0.0f, 1.0f );
	glScalef( length, width, height );
	glutSolidCube( 1 );
	glPopMatrix();
}
