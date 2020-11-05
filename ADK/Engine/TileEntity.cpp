#include "TileEntity.h"

TileEntity::TileEntity()
{
	use_origin_for_position(false);

	set_frame_size(16, 16);
}

void TileEntity::set_frame_size(int x, int y)
{
	Entity::set_frame_size(x, y);

	// Set origin to middle
	set_origin(sprite_sheet.frame_size.x / 2.f, sprite_sheet.frame_size.y / 2.f);
}
