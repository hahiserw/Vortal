/*
 * Wiezyczka
 */

class Turrent: virtual public Obstacle
{
	bool active;
	float fire_time_start, fire_time_end;
public:
	Turrent( float, float, float );
	void draw() const;
	void activate();
	void deactivate();
	void tick( float, float, float );
	void fire( float );
};

Turrent::Turrent( float x, float y, float z )
{
	this->x = x;
	this->y = y;
	this->z = z;
	length = 0.6;
	width = 0.3;
	height = 0.7;
	rotation = 90;
	active = false;
}

void Turrent::activate()
{
	if( active )
		return;
	active = true;
}

void Turrent::deactivate()
{
	if( !active )
		return;
	active = false;
}

void Turrent::tick( float moveX, float moveY, float now )
{
	active =
		x - width < moveX && moveX < x + width
		;//&& y -5> moveY && moveY > y;

	if( active ) {
		fire( now );
	}
}

void Turrent::fire( float now )
{
	fire_time_start = now;
	if( fire_time_start < fire_time_end )
		return;
	
	glMaterialfv( GL_FRONT, GL_AMBIENT, colorDim );
	glMaterialfv( GL_FRONT, GL_EMISSION, shootColor );
	glMaterialfv( GL_FRONT, GL_DIFFUSE, shadowColor );
	glMaterialfv( GL_FRONT, GL_SPECULAR, colorDim );
	glPushMatrix();
	glTranslatef( x, y, 0.5 );
	glScalef( 5, 0.1, 0.1 );
	glutSolidCube( 1 );
	glPopMatrix();
	fire_time_end = fire_time_start + 100;
}

void Turrent::draw() const
{

	glMaterialfv( GL_FRONT, GL_AMBIENT, colorDim ); //dim
	if( active )
		glMaterialfv( GL_FRONT, GL_EMISSION, turrentColor );
	else
		glMaterialfv( GL_FRONT, GL_EMISSION, shadowColor );
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
