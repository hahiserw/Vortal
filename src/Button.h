/*
 * Przycisk obci¹¿eniowy aktywowany kostk¹
 */

//class ExitDoor;

class Button: virtual public Obstacle
{
	bool active;
	bool blocked;
	ExitDoor * slave;
public:
	Button::Button( float, float, float, ExitDoor * );
	void draw() const;
	void activate();
	void deactivate();
	bool isActivated();
	void block();
	void unblock();
	void blockToggle();
	bool isBlocked();
};

Button::Button( float x, float y, float z, ExitDoor * slave )
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
	this->slave = slave;
}

void Button::activate()
{
	if( active || blocked )
		return;
	active = true;
	z -= 0.09f;
	slave->activate();
}

void Button::deactivate()
{
	if( !active || blocked )
		return;
	active = false;
	z += 0.09f;
	slave->deactivate();
}

bool Button::isActivated()
{
	return active;
}

void Button::block()
{
	blocked = true;
}

void Button::unblock()
{
	blocked = false;
}

void Button::blockToggle()
{
	blocked = !blocked;
}

bool Button::isBlocked()
{
	return blocked;
}


void Button::draw() const
{

	glMaterialfv( GL_FRONT, GL_AMBIENT, colorDim ); //dim
	glMaterialfv( GL_FRONT, GL_EMISSION, buttonColor );
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
