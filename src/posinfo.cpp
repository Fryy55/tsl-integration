#include "include/PosDisplay.hpp"
#include "include/LevelInfo.hpp"

#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>


class $modify(LevelInfoLayer) {
	struct Fields {
		PosDisplay* m_tslPos;
		PosDisplay* m_tslpPos;
		EventListener<EventFilter<LevelInfo::InitEvent>> m_initListener;
	};

	$override bool init(GJGameLevel* level, bool challenge) {
		if (!LevelInfoLayer::init(level, challenge))
			return false;

		m_fields -> m_initListener = { [this](LevelInfo::InitEvent*) {
			log::debug("Refreshing position nodes...");
			this -> initPos();

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
		menu -> setLayout(AnchorLayout::create());
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
				title -> getPositionX() - (title -> getContentWidth() / 2) + 20.f,
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

		m_fields -> m_tslPos = tslPos;
		m_fields -> m_tslpPos = tslpPos;
	}

	$override void onUpdate(CCObject* sender) {
		LevelInfoLayer::onUpdate(sender);

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