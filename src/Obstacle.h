/*
 * Przeszkoda :D
 */

class Obstacle
{
	float x, y;
	float width, height;
public:
	Obstacle() {};
	Obstacle( float, float, float, float );
	void moveTo( float, float );
	float getX() const { return x; }
	float getY() const { return y; }
	float getWidth() const { return width; }
	float getHeight() const { return height; }
	void setX( float value ) { x = value; }
	void setY( float value ) { y = value; }
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
	//cout << "move to " << x << ", " << y << endl;
	this->x = x;
	this->y = y;
}

void Obstacle::draw() const
{
	glPushMatrix();
	glTranslatef( x + width/2.0f, y + height/2.0f, 0.0f ); // Bo skalowanie jest wzgledem srodka
	//glTranslatef( (float)-width/2.0f, (float)-height/2.0f, 0.0f );
	glScalef( width, height, 1.0f );
	glutSolidCube( 1 );
	glPopMatrix();
}
