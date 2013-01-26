/*
 * Drzwi wyjœciowe
 */

class ExitDoor: virtual public Obstacle
{
	bool active;
public:
	ExitDoor( float, float, float );
	void draw() const;
	void activate();
	void deactivate();
};

ExitDoor::ExitDoor( float x, float y, float z )
{
	this->x = x + 0.45f; // Nie?
	this->y = y;
	this->z = z;
	length = 0.3;
	width = 1;
	height = 1.5;
	rotation = 90;
	active = false;
}

void ExitDoor::activate()
{
	if( active )
		return;
	active = true;
	x -= 1.2f;
}

void ExitDoor::deactivate()
{
	if( !active )
		return;
	active = false;
	x += 1.2f;
}


void ExitDoor::draw() const
{

	glMaterialfv( GL_FRONT, GL_AMBIENT, colorDim ); //dim
	glMaterialfv( GL_FRONT, GL_EMISSION, doorColor );
	glMaterialfv( GL_FRONT, GL_DIFFUSE, shadowColor );
	glMaterialfv( GL_FRONT, GL_SPECULAR, colorDim );

	glPushMatrix();
	glTranslatef( x + length/2.0f, y + width/2.0f, z + height/2.0f ); // Bo skalowanie jest wzgledem srodka
	//glTranslatef( (float)-width/2.0f, (float)-height/2.0f, 0.0f );
	//glRotatef( globalRotation, 0.0f, 0.0f, 1.0f );
	glRotatef( rotation, 0.0f, 0.0f, 1.0f );
	glScalef( length, width, height );
	glutSolidCube( 1 );
	glPopMatrix();
}
