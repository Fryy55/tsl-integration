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

void LevelInfo::uninit() {
	m_init = false;

	return;
}

void LevelInfo::refresh() {
	if (!m_manual && m_reqMutex) // :skull:
		return;
	m_reqMutex = true;

	FryyScheduler::unscheduleForwarding(schedule_selector(LevelInfo::refreshForward));
	FryyScheduler::unscheduleForwarding(schedule_selector(LevelInfo::retryLoopForward));

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

void LevelInfo::manualRefresh() {
	if (m_reqMutex) {
		Notification::create(
			"An API request is already processing...",
			NotificationIcon::Error,
			2.f
		) -> show();
		return;
	}
	m_reqMutex = true;

	m_manual = true;
	m_retry = 2;
	log::debug("Request type: manual.");


	m_refreshIndicator = CCMenu::create();
	m_refreshIndicator -> setPosition(550.f, 300.f);
	m_refreshIndicator -> setID("refresh-indicator"_spr);

	auto circle = LoadingSpinner::create(30.f);
	circle -> setID("circle"_spr);
	auto x = CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
	x -> setVisible(false);
	x -> setID("x"_spr);
	auto v = CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png");
	v -> setVisible(false);
	v -> setID("v"_spr);

	m_refreshIndicator -> addChild(circle);
	m_refreshIndicator -> addChild(x);
	m_refreshIndicator -> addChild(v);
	m_refreshIndicator -> setOpacity(0);

	SceneManager::get() -> keepAcrossScenes(m_refreshIndicator);
	m_refreshIndicator -> runAction(CCFadeIn::create(0.2f));

	this -> refresh();

	return;
}

LevelInfo::LevelInfo() : m_retry(2), m_init(false), m_manual(false), m_reqMutex(false) {
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
			if (m_manual) {
				indicatorUpdate(false); // releases mutex

				m_manual = false;
			} else {
				m_reqMutex = false;
			}

			m_lastErr = std::make_pair<char const*, char const*>(std::move(err1), std::move(err2));
			FryyScheduler::periodic(schedule_selector(LevelInfo::retryLoopForward), 3.f);
			FryyScheduler::once(schedule_selector(LevelInfo::retryLoopForward), 0.2f); // technically this over normal call should improve uhh starvation???? idk mayhaps it helps :thumbsup:
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
		log::info("Level information successfully parsed.");
		if (!m_init) {
			m_init = true;
			InitEvent().post();
			log::info("Level information initialized.");
		} else {
			RefreshedEvent().post();
			log::info("Level information refreshed.");
		}
		FryyScheduler::once(schedule_selector(LevelInfo::refreshForward), 1800.f);
		log::debug("Level information refresh scheduled; Next refresh is in 30 minutes.");
		if (m_manual) {
			indicatorUpdate(true); // releases mutex

			m_manual = false;
		} else
			m_reqMutex = false;
	} else if (event -> isCancelled()) {
		log::info("API call for level information cancelled.");
		// no idea what i'm doing here it should be dead code tbh
		m_reqMutex = false;
		m_manual = false;
		m_retry = 2;
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

void LevelInfo::indicatorUpdate(bool success) {
	auto mark = success ? m_refreshIndicator -> getChildByID("v"_spr) : m_refreshIndicator -> getChildByID("x"_spr);
	m_refreshIndicator -> getChildByID("circle"_spr) -> setVisible(false);
	mark -> setVisible(true);

	mark -> runAction(CCSequence::createWithTwoActions(
		CCDelayTime::create(0.5f),
		CCFadeOut::create(0.5f)
	));
	FryyScheduler::once(schedule_selector(LevelInfo::killRefreshIndicatorForward), 1.f);

	return;
}

void LevelInfo::killRefreshIndicator() {
	SceneManager::get() -> forget(m_refreshIndicator);
	m_refreshIndicator -> removeFromParent();
	
	m_reqMutex = false;

	return;
}