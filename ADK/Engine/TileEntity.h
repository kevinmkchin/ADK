#pragma once
#include "Entity.h"


/** Represents an entity with position origin at the top left corner of the sprite, 
	but rotation and scale origin at the center of the sprite. */
class TileEntity : public Entity
{
public:
	TileEntity();

protected:
	virtual void set_frame_size(int x, int y) override;

};

