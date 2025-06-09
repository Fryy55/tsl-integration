#include "LevelInfo.hpp"
#include "FryyScheduler.hpp"


LevelInfo* LevelInfo::get() {
	static LevelInfo instance;

	return &instance;
}

LevelInfo::Level const& LevelInfo::getLevelById(unsigned int id) const {
	return m_db.at(id);
}

bool LevelInfo::isInDBById(unsigned int id) const {
	return m_db.contains(id);
}

bool LevelInfo::isInitialized() const {
	return m_init;
}

void LevelInfo::refresh() {
	FryyScheduler::unscheduleForwarding(schedule_selector(LevelInfo::refreshForward));
	FryyScheduler::unscheduleForwarding(schedule_selector(LevelInfo::retryLoopForward));
	m_retry = 2;

	web::WebRequest req;
	req
		.timeout(std::chrono::seconds(10))
		.userAgent(
			fmt::format(
				"TSL Integration {} {}/GD{}",
				Mod::get() -> getVersion(),
				GEODE_PLATFORM_NAME,
				Loader::get() -> getGameVersion()
			)
		);

	log::debug("Requesting level information...");
	m_listener.setFilter(req.get("https://tslwithbackend-production.up.railway.app/api/ordered-ids"));

	return;
}

LevelInfo::LevelInfo() : m_retry(2), m_init(false) {
	m_listener.bind(this, &LevelInfo::onAPIRequest);
}

void LevelInfo::onAPIRequest(web::WebTask::Event* event) {
	if (auto val = event -> getValue()) {
		auto retry = [this](char const* err1, char const* err2) {
			log::warn(
				"{} {} Waiting for a retry opportunity (scheduled retry time: {} minutes).",
				err1,
				err2,
				m_retry
			);

			m_lastErr = std::make_pair<char const*, char const*>(std::move(err1), std::move(err2));
			FryyScheduler::periodic(schedule_selector(LevelInfo::retryLoopForward), 3.f);
		};

		log::debug("Level information fetched.");

		if (!val -> ok()) {
			retry("Level information request failed.", "Check your internet connection.");
			return;
		}

		GEODE_UNWRAP_OR_ELSE(json, err, val -> json()) {
			retry("Error converting the level", "information request.");
			return;
		}

		if (!json.isArray()) {
			retry("Error parsing the level information", "request: expected an array.");
			return;
		}
		unsigned short curPos = 1;
		for (auto const& levelObject : json) { // reimplement this later pls :pray:
			Level level;
			level.Pos = curPos;
			GEODE_UNWRAP_OR_ELSE(id, err, levelObject.asUInt()) {
				log::error("Error parsing level information for level #{}. This will not be retried until the next planned retry.", curPos);
				continue;
			}
			m_db[id] = std::move(level);
			++curPos;
		}
		m_retry = 2;
		if (!m_init) {
			m_init = true;
			InitEvent().post();
			log::info("Level information initialized.");
		}
		log::info("Level information successfully parsed.");
		FryyScheduler::once(schedule_selector(LevelInfo::refreshForward), 1800.f);
		log::debug("Level information refresh scheduled; Next refresh is in 30 minutes.");
	} else if (event -> isCancelled()) {
		log::info("API call for level information cancelled.");
	}
}

void LevelInfo::retryLoop() {
	if (CCDirector::get() -> getRunningScene() -> getChildByID("PlayLayer") != nullptr)
		return;

	FryyScheduler::unscheduleForwarding(schedule_selector(LevelInfo::retryLoopForward));

	log::info("Retrying in {} minutes for error \"{} {}\".", m_retry, m_lastErr.first, m_lastErr.second);
	auto msg = fmt::format(
		"  [TSL Integration]\n   | {}\n   |   {}\n   | Retrying in {} minutes.",
		m_lastErr.first,
		m_lastErr.second,
		m_retry
	);
	if (Mod::get() -> getSettingValue<bool>("level-info-request-fail-notifs"))
		Notification::create(msg, NotificationIcon::Warning, 5.f) -> show();

	FryyScheduler::once(schedule_selector(LevelInfo::refreshForward), 60.f * m_retry);

	switch (m_retry) {
		case 2:
			m_retry = 3;
			break;
		case 3:
			m_retry = 5;
			break;
		case 5:
			m_retry = 10;
			if (!Mod::get() -> setSavedValue("shown-notif-help", true))
				FLAlertLayer::create(
					"Annoyed by retries?",
					"If you don't want to see these notifications, you can hide them in mod's <cy>settings</c>.",
					"OK"
				) -> show();

			break;
		default:
			break;
	}

	return;
}