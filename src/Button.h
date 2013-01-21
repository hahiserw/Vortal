/*
 * Przycisk obci¹¿eniowy aktywowany kostk¹
 */

//class ExitDoor;

class Button: public Obstacle
{
	bool active;
	bool blocked;
	ExitDoor * slave;
public:
	Button::Button( float, float, float, ExitDoor * );
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
	z -= 0.1f;
	slave->activate();
}

void Button::deactivate()
{
	if( !active || blocked )
		return;
	active = false;
	z += 0.1f;
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