#include "include/PosDisplay.hpp"
#include "include/LevelInfo.hpp"
#include "include/RefreshLayer.hpp"

#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>


class $modify(HLevelInfoLayer, LevelInfoLayer) {
	struct Fields {
		CCMenu* m_tslMenu;
		EventListener<EventFilter<LevelInfo::InitEvent>> m_initListener;
		EventListener<EventFilter<LevelInfo::RefreshedEvent>> m_refreshedListener;
	};

	$override bool init(GJGameLevel* level, bool challenge) {
		if (!LevelInfoLayer::init(level, challenge))
			return false;

		m_fields -> m_initListener = { [this](LevelInfo::InitEvent*) {
			log::debug("Initializing visible position nodes...");
			this -> initPos();

			return ListenerResult::Propagate;
		} };
		m_fields -> m_refreshedListener = { [this](LevelInfo::RefreshedEvent*) {
			log::debug("Refreshing position nodes...");
			this -> refreshPos();

			return ListenerResult::Propagate;
		} };

		if (LevelInfo::get() -> isInitialized())
			this -> initPos();

		return true;
	}

	void initPos() {
		bool assumed = false;
		if(!LevelInfo::get() -> isInDBById(m_level -> m_levelID)) {
			if(
				utils::string::toLower(m_level -> m_levelName).find("shitty") != std::string::npos
				||
				utils::string::toLower(m_level -> m_levelName).find("sh ") != std::string::npos
			)
				assumed = true;
			else
				return;
		}

		auto normalModeLabel = this -> getChildByID("normal-mode-label");

		auto tslPos = PosDisplay::create(PosDisplay::TSL, m_level, assumed);
		tslPos -> setID("tsl-icon"_spr);
		auto tslpPos = PosDisplay::create(PosDisplay::TSLP, m_level, assumed);
		tslpPos -> setID("tslp-icon"_spr);

		auto menu = CCMenu::create();
		menu -> addChildAtPosition(
			tslPos,
			Anchor::Center,
			{ -100.f, -8.f }
		);
		menu -> addChildAtPosition(
			tslpPos,
			Anchor::Center,
			{ 100.f, -8.f }
		);
		menu -> setID("tsl-menu"_spr);
		this -> addChild(menu);

		if (assumed) {
			auto assumedInfoBtnSpr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
			assumedInfoBtnSpr -> setScale(0.7f);
			auto assumedInfoBtn = CCMenuItemSpriteExtra::create(
				assumedInfoBtnSpr,
				this,
				menu_selector(PosDisplay::onAssumedInfo)
			);
			assumedInfoBtn -> setID("assumed-info-button"_spr);

			menu -> addChild(assumedInfoBtn);
			auto title = this -> getChildByID("title-label");
			assumedInfoBtn -> setPosition(
				title -> getPositionX()
				-
				(title -> getContentWidth() / 2 * title -> getScale())
				-
				10.f,
				title -> getPositionY() - 2.f
			);

			assumedInfoBtn -> setOpacity(0);
			assumedInfoBtn -> runAction(
				CCSequence::createWithTwoActions(
					CCDelayTime::create(0.6f),
					CCFadeIn::create(0.6f)
				)
			);
		}

		m_fields -> m_tslMenu = menu;
	}

	void initPosForward(float) {
		this -> initPos();

		return;
	}

	void refreshPos() {
		if (m_fields -> m_tslMenu) {
			m_fields -> m_tslMenu -> runAction(CCFadeOut::create(0.15f));
			m_fields -> m_tslMenu -> scheduleOnce(schedule_selector(CCNode::removeFromParent), 0.15f);
		}
		this -> scheduleOnce(schedule_selector(HLevelInfoLayer::initPosForward), 0.15f);

		return;
	}

	$override void onUpdate(CCObject* sender) {
		if (!(static_cast<CCNode*>(sender) -> getUserObject("orig"_spr))) {
			auto refreshBtn = this -> getChildByID("right-side-menu") -> getChildByID("refresh-button"); // sender can be a coverup button
			auto rightMenu = this -> getChildByID("right-side-menu");
			this -> m_pParent -> addChild(
				RefreshLayer::create(
					rightMenu -> getPositionX(),
					rightMenu -> getPositionY()
					+
					refreshBtn -> getPositionY()
					-
					rightMenu -> getContentHeight() / 2
				), 100
			);
		} else {
			LevelInfoLayer::onUpdate(nullptr);
		}

		return;
	}
};

class $modify(MenuLayer) {
	$override bool init() {
		if (!MenuLayer::init())
			return false;

		static bool init = false;
		if (!init) {
			LevelInfo::get() -> refresh();
			init = true;
		}

		return true;
	}
};