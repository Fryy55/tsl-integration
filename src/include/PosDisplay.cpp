#include "PosDisplay.hpp"


PosDisplay* PosDisplay::create(List list, GJGameLevel* level, bool assumed) {
	auto ret = new PosDisplay;
	
	if (ret -> init(list, level, assumed)) {
		ret -> autorelease();
		return ret;
	}

	delete ret;
	return nullptr;
}

void PosDisplay::onAssumedInfo(CCObject* sender) {


	return;
}

void PosDisplay::onConcretePosition(CCObject* sender) {


	return;
}

bool PosDisplay::init(List list, GJGameLevel* level, bool assumed) {
	if (!CCSprite::initWithSpriteFrameName(list ? "tslp_trophy.png"_spr : "tsl_trophy.png"_spr)) // TSL+ : TSL
		return false;

	unsigned int const id = level -> m_levelID;
	if (list == TSL) {
		if (LevelInfo::get() -> isInDBById(id))
			m_pos = CCLabelBMFont::create(std::to_string(LevelInfo::get() -> getLevelById(id).Pos).c_str(), "goldFont.fnt");
		else
			m_pos = CCLabelBMFont::create(" - ", "goldFont.fnt");
	} else { // TSL+
		/* if (LevelInfo::get() -> isInDBByIdP(id))
			m_pos = CCLabelBMFont::create(std::to_string(LevelInfo::get() -> getLevelByIdP(id).Pos).c_str(), "goldFont.fnt");
		else */
			m_pos = CCLabelBMFont::create(" - ", "goldFont.fnt");
	}
	m_pos -> setID("position"_spr);

	this -> setScale(0.85f);
	m_pos -> setScale(0.7f);

	m_posBtn = assumed ?
		CCMenuItemSpriteExtra::create(
			m_pos,
			this,
			menu_selector(PosDisplay::onAssumedInfo)
		)
		:
		CCMenuItemSpriteExtra::create(
			m_pos,
			this,
			menu_selector(PosDisplay::onConcretePosition)
		);
	m_posBtn -> setTag(id);
	m_posBtn -> setID("position-button"_spr);
	m_posMenu = CCMenu::create();
	m_posMenu -> setID("position-menu"_spr);
	m_list = list;
	m_level = level;

	if (list == TSL) {
		this -> setAnchorPoint({ 1.f, 0.5f });
		m_posMenu -> setAnchorPoint({ 0.f, 0.5f });

		m_posMenu -> addChildAtPosition(
			m_posBtn,
			Anchor::Left,
			{ m_posBtn -> getContentWidth() / 2 + 5.f, 0 }
		);
		this -> addChildAtPosition(m_posMenu, Anchor::Right);
	} else { // TSL+
		this -> setAnchorPoint({ 0.f, 0.5f });
		m_posMenu -> setAnchorPoint({ 1.f, 0.5f });

		m_posMenu -> addChildAtPosition(
			m_posBtn,
			Anchor::Right,
			{ -(m_posBtn -> getContentWidth() / 2 + 5.f), 0 }
		);
		this -> addChildAtPosition(m_posMenu, Anchor::Left);
	}

	this -> setCascadeOpacityEnabled(true);
	this -> setOpacity(0);
	this -> runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.3f), CCFadeIn::create(0.3f)));

	return true;
}