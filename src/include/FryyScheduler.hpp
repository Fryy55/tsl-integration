#include <Geode/Geode.hpp>

using namespace geode::prelude;


class FryyScheduler final : public CCNode {
public:
	static FryyScheduler* get();

	static void once(SEL_SCHEDULE forwardingCallback, float delay);
	static void periodic(
		SEL_SCHEDULE forwardingCallback,
		float interval = 0,
		unsigned int repeat = kCCRepeatForever,
		float delay = 0
	);

	static void unscheduleForwarding(SEL_SCHEDULE forwardingCallback);
	static void unscheduleAll();

private:
	FryyScheduler();
	// Kill all semantics because they bad
	FryyScheduler(FryyScheduler const&) = delete;
	FryyScheduler& operator=(FryyScheduler const&) = delete;
	FryyScheduler(FryyScheduler&&) = delete;
	FryyScheduler& operator=(FryyScheduler&&) = delete;
	~FryyScheduler() override {}
};