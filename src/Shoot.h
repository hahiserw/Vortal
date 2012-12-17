/*
 * Strza³ ;3
 */

class Shoot
{
	float x, y;
	float dir;
	float time;
public:
	Shoot( float, float, float, float );
};

Shoot::Shoot( float x, float y, float dir, float time )
{
	this->x = x;
	this->y = y;
	this->dir = dir;
	this->time = time;
}