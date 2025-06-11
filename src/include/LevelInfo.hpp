#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;


class LevelInfo final {
public:
	struct Level {
		unsigned short Pos;
		unsigned int LdmId;
		char const* Name;
		char const* Publisher;
		std::vector<char const*> Creators;
		char const* Verifier;
		char const* Verification;
		char const* Showcase;
		unsigned int Difficulty : 4;
		unsigned int Rating : 4;
	};

	class InitEvent : public Event {};
	class RefreshedEvent : public Event {};

	static LevelInfo* get();

	Level const& getLevelById(unsigned int id) const;
	bool isInDBById(unsigned int id) const;
	bool isInitialized() const;
	void uninit();

	void refresh();
	void refreshForward() const { LevelInfo::get() -> refresh(); }
	void manualRefresh();

private:
	LevelInfo();
	// Kill all semantics because they bad
	LevelInfo(LevelInfo const&) = delete;
	LevelInfo& operator=(LevelInfo const&) = delete;
	LevelInfo(LevelInfo&&) = delete;
	LevelInfo& operator=(LevelInfo&&) = delete;
	// Rule of 5
	~LevelInfo() {}

	void onAPIRequest(web::WebTask::Event*);
	void retryLoop();
	void retryLoopForward() const { LevelInfo::get() -> retryLoop(); }
	void indicatorUpdate(bool);
	void killRefreshIndicator();
	void killRefreshIndicatorForward() { LevelInfo::get() -> killRefreshIndicator(); }

	// Fields
	std::unordered_map<unsigned int, Level> m_db;
	bool m_init;
	bool m_manual;
	EventListener<web::WebTask> m_listener;
	CCMenu* m_refreshIndicator;
	unsigned char m_retry;
	std::pair<char const*, char const*> m_lastErr;
	bool m_reqMutex;
};