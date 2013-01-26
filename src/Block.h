/*
 * Bloki do budowania planszy
 */

class Block: virtual public Obstacle
{
public:
	Block() {}
	Block( float, float, float, float, float, float );
	void draw() const;
};

Block::Block( float x, float y, float z, float length, float width, float height )
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->length = length;
	this->width = width;
	this->height = height;
	this->rotation = 0;
}

void Block::draw() const
{
	/*
	float col[] = { 0.2, 0.2, 0.2, 1 };
	glMaterialfv( GL_FRONT, GL_AMBIENT, col ); //dim
	col[0] = col[1] = col[2] = 0;
	glMaterialfv( GL_FRONT, GL_EMISSION, col );
	col[0] = col[1] = col[2] = 0.8;
	glMaterialfv( GL_FRONT, GL_DIFFUSE, col );
	col[0] = col[1] = col[2] = 0;
	glMaterialfv( GL_FRONT, GL_SPECULAR, col );
	glEnable(GL_CULL_FACE);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_TEXTURE_2D);
	/*/
	glMaterialfv( GL_FRONT, GL_AMBIENT, colorDim ); //dim
	glMaterialfv( GL_FRONT, GL_EMISSION, boardColor );
	glMaterialfv( GL_FRONT, GL_DIFFUSE, shadowColor );
	glMaterialfv( GL_FRONT, GL_SPECULAR, colorDim );
	//*/
	glPushMatrix();
	glTranslatef( x + length/2.0f, y + width/2.0f, z + height/2.0f ); // Bo skalowanie jest wzgledem srodka
	//glTranslatef( (float)-width/2.0f, (float)-height/2.0f, 0.0f );
	//glRotatef( globalRotation, 0.0f, 0.0f, 1.0f );
	glRotatef( rotation, 0.0f, 0.0f, 1.0f );
	glScalef( length, width, height );
	glutSolidCube( 1 );
	glPopMatrix();

	//glDisable(GL_TEXTURE_2D);
}
