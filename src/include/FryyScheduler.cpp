#include "FryyScheduler.hpp"


FryyScheduler* FryyScheduler::get() {
	static FryyScheduler* instance = new FryyScheduler;

	return instance;
}

void FryyScheduler::once(SEL_SCHEDULE forwardingCallback, float delay) {
	FryyScheduler::get() -> scheduleOnce(forwardingCallback, delay);
}

void FryyScheduler::periodic(
	SEL_SCHEDULE forwardingCallback,
	float interval,
	unsigned int repeat,
	float delay
) {
	if (interval == 0)
		FryyScheduler::get() -> schedule(forwardingCallback);
	else if (repeat == kCCRepeatForever)
		FryyScheduler::get() -> schedule(forwardingCallback, interval);
	else
		FryyScheduler::get() -> schedule(forwardingCallback, interval, repeat, delay);

	return;
}

void FryyScheduler::unscheduleForwarding(SEL_SCHEDULE forwardingCallback) {
	FryyScheduler::get() -> unschedule(forwardingCallback);

	return;
}

void FryyScheduler::unscheduleAll() {
	FryyScheduler::get() -> unscheduleAllSelectors();

	return;
}

FryyScheduler::FryyScheduler() {
	this -> onEnter();
}