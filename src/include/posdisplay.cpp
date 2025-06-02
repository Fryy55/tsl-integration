#include "posdisplay.hpp"

using namespace geode::prelude;


PosDisplay* PosDisplay::create(List list) {
	auto ret = new PosDisplay;
	
	if (ret -> init(list)) {
		ret -> autorelease();
		return ret;
	}

	delete ret;
	return nullptr;
}

bool PosDisplay::init(List list) {
	if (!this -> CCMenu::init())
		return false;
	this -> setLayout(RowLayout::create());

	m_icon = CCSprite::createWithSpriteFrameName(list ? "tslp_trophy.png"_spr : "tsl_trophy.png"_spr); // TSL+ : TSL
	m_pos = CCLabelBMFont::create("-", "goldFont.fnt");

	this -> addChild(m_icon);
	this -> addChild(m_pos);
	this -> setContentWidth(m_icon -> getContentWidth() + m_pos -> getContentWidth() + 5.f);
	this -> updateLayout();
	this -> setScale(0.6f);

	this -> setOpacity(0);
	this -> runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.5f), CCFadeIn::create(0.3f)));

	return true;
}