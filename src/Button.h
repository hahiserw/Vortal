/*
 * Przycisk obci¹¿eniowy aktywowany kostk¹
 */

//class ExitDoor;

class Button: public Obstacle
{
	ExitDoor * slave;
public:
	Button::Button( float, float, float, ExitDoor * );
	void activate();
	void deactivate();
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
	this->slave = slave;
}

void Button::activate()
{
	slave->open();
}

void Button::deactivate() {}