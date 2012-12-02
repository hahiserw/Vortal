/*
 * Przeszkoda :D
 */

class Obstacle
{
	float x, y;
	float width, height;
public:
	Obstacle( float, float, float, float );
	void moveTo( float, float );
	float getX() const { return x; }
	float getY() const { return y; }
	float getWidth() const { return width; }
	float getHeight() const { return height; }
	void draw() const;
};

Obstacle::Obstacle( float x, float y, float width, float height )
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
}

void Obstacle::moveTo( float x, float y )
{
	this->x = x;
	this->y = y;
}

void Obstacle::draw() const
{
	glPushMatrix();
	glTranslatef( x, y, 1.0f );
	//glTranslatef( (float)-width/2.0f, (float)-height/2.0f, 0.0f );
	glScalef( width, height, 1.0f );
	glutSolidCube( 1 );
	glPopMatrix();
}