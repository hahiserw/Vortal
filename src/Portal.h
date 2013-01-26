/*
 * Klasa na oba portale
 */

class Portal: virtual public Obstacle
{
	bool active;
	bool blocked;
	Portal *second;
public:
	Portal( float, float, float, Portal * );
	//friend class Portal;
	void draw() const;
	void activate();
	void deactivate();
	bool isActivated();
	void block();
	void unblock();
	void blockToggle();
	bool isBlocked();
};

Portal::Portal( float x, float y, float z, Portal *second )
{
	this->x = x;
	this->y = y;
	this->z = z;
	length = 0.9;
	width = 0.9;
	height = 0.1;
	rotation = 0;
	active = false;
	blocked = false;
	this->second = second;
}


void Portal::draw() const
{
	if( !active )
		return;

	glMaterialfv( GL_FRONT, GL_AMBIENT, colorDim ); //dim
	if( active )
		glMaterialfv( GL_FRONT, GL_EMISSION, buttonColor );
	else
		glMaterialfv( GL_FRONT, GL_EMISSION, colorDim );
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
