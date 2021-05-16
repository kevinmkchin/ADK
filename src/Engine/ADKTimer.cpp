#include "ADKTimer.h"

ADKTimer* ADKTimer::instance = nullptr;

ADKTimer::ADKTimer()
{
}

ADKTimer* ADKTimer::get_timer()
{
	if (instance == nullptr)
	{
		instance = new ADKTimer();
	}

	return instance;
}

void ADKTimer::update_timer(float delta_time)
{
	for (std::size_t i = 0; i < callbacks.size(); ++i)
	{
		ADKTimedCallback& timed_callback = callbacks.at(i);
		timed_callback.timer -= delta_time;
		if (timed_callback.timer <= 0.f)
		{
			// Call callback
			std::invoke(timed_callback.stored_func);

			// Remove from vector
			callbacks.erase(callbacks.begin() + i);
			--i;
		}
	}
}
