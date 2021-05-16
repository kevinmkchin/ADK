#include "PlatformerDamageBox.h"
#include "PlatformerPlayer.h"

void PlatformerDamageBox::collided(Entity* collided_entity)
{
	if (PlatformerPlayer* player = dynamic_cast<PlatformerPlayer*>(collided_entity))
	{
		player->affect_health(-1.f);
	}
}
