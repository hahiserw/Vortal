/*
 * Przeszkoda :D
 */

class Obstacle
{
protected:
	int texture;
	float x, y, z;
	float length, width, height;
	float rotation; // K¹t w stopniach
public:
	Obstacle() {};
	virtual ~Obstacle() {};
	void moveTo( float, float );
	void rotate( float );
	float getX() const { return x; }
	float getY() const { return y; }
	float getWidth() const { return width; }
	float getHeight() const { return height; }
	void setX( float value ) { x = value; }
	void setY( float value ) { y = value; }
	void set_texture( int value ) { texture = value; }
	virtual void draw() const = 0;
};

void Obstacle::moveTo( float x, float y )
{
	this->x = x;
	this->y = y;
}

void Obstacle::rotate( float rotation )
{
	this->rotation = rotation;
}
