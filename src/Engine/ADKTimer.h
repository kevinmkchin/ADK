#pragma once
#include <vector>
#include <functional>

struct ADKTimedCallback
{
	// time left until we call stored_func
	float timer = -1.f;

	// stored function, bound to an object
	std::function<void()> stored_func;
};

/*	ADKTimer is a singleton class */
class ADKTimer
{

private:
	/* Here will be the instance stored. */
	static ADKTimer* instance;

	/* Private constructor to prevent instancing. */
	ADKTimer();

public:
	/* Static access method. */
	static ADKTimer* get_timer();

	/* registers a new ADKTimedCallback */
	template<typename T>
	void set_timed_callback(void(T::*func)(), T* obj_ptr, float time);

	void update_timer(float delta_time);

private:
	/* Stores callbacks */
	std::vector<ADKTimedCallback> callbacks;

};

template<typename T>
void ADKTimer::set_timed_callback(void(T::*func)(), T* obj_ptr, float time)
{
	ADKTimedCallback callback;
	callback.timer = time;
	callback.stored_func = std::bind(func, obj_ptr);
	callbacks.push_back(callback);
}
