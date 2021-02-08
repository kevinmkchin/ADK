#include "TileEntity.h"

ADKOBJECT(TileEntity)
TileEntity::TileEntity()
{
	ADKOBJECT_BEGIN(TileEntity)
	ADKOBJECT_END()

	use_origin_for_position(false);

	set_frame_size(16, 16);
}