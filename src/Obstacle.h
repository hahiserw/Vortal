/*
 * Przeszkoda :D
 */

class Obstacle
{
protected:
	float x, y, z;
	float length, width, height;
	float rotation; // K¹t w stopniach
public:
	Obstacle() {};
	Obstacle( float, float, float, float, float, float );
	void moveTo( float, float );
	void rotate( float );
	float getX() const { return x; }
	float getY() const { return y; }
	float getWidth() const { return width; }
	float getHeight() const { return height; }
	void setX( float value ) { x = value; }
	void setY( float value ) { y = value; }
	void draw() const;
};

Obstacle::Obstacle( float x, float y, float z, float length, float width, float height )
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->length = length;
	this->width = width;
	this->height = height;
	this->rotation = 0;
}

void Obstacle::moveTo( float x, float y )
{
	//cout << "move to " << x << ", " << y << endl;
	this->x = x;
	this->y = y;
}

void Obstacle::rotate( float rotation )
{
	this->rotation = rotation;
}

void Obstacle::draw() const
{
	glPushMatrix();
	glTranslatef( x + length/2.0f, y + width/2.0f, z + height/2.0f ); // Bo skalowanie jest wzgledem srodka
	//glTranslatef( (float)-width/2.0f, (float)-height/2.0f, 0.0f );
	//glRotatef( globalRotation, 0.0f, 0.0f, 1.0f );
	glRotatef( rotation, 0.0f, 0.0f, 1.0f );
	glScalef( length, width, height );
	glutSolidCube( 1 );
	glPopMatrix();
}
