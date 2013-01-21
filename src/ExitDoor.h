/*
 * Drzwi wyjœciowe z planszy
 */

class ExitDoor: public Obstacle
{
	bool active;
public:
	ExitDoor( float, float, float );
	void activate();
	void deactivate();
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
	active = false;
}

void ExitDoor::activate()
{
	if( active )
		return;
	active = true;
	x -= 1.2f;
}

void ExitDoor::deactivate()
{
	if( !active )
		return;
	active = false;
	x += 1.2f;
}