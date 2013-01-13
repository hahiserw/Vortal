/*
 * Drzwi wyjœciowe z planszy
 */

class ExitDoor: public Obstacle
{
public:
	ExitDoor( float, float, float );
	void open();
	void close();
};

ExitDoor::ExitDoor( float x, float y, float z )
{
	this->x = x + 0.5f; // Nie?
	this->y = y;
	this->z = z;
	length = 0.3;
	width = 1;
	height = 0.9;
	rotation = 90;
}

void ExitDoor::open()
{
	z = -0.999;
}

void ExitDoor::close()
{
	z = 1;
}