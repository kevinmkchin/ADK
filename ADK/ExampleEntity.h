#pragma once

#include "Engine/Entity.h"

class ExampleEntity : public Entity
{
public:
	ExampleEntity();

	virtual void InitCollider() override;

};

