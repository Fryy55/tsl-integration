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

	static LevelInfo* get();

	Level const& getLevelById(unsigned int id) const;
	bool isInDBById(unsigned int id) const;
	bool isInitialized() const;

	void refresh();
	void refreshForward() const { LevelInfo::get() -> refresh(); }

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

	// Fields
	std::unordered_map<unsigned int, Level> m_db;
	bool m_init;
	EventListener<web::WebTask> m_listener;
	unsigned char m_retry;
	std::pair<char const*, char const*> m_lastErr;
};