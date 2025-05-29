#include "posdisplay.hpp"


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
	this -> CCMenu::init();
	this -> setLayout(RowLayout::create());

	m_icon = CCSprite::createWithSpriteFrameName("rankIcon_top500_001.png");
	m_icon -> setColor(list ? ccColor3B(255, 0, 0) : ccColor3B(36, 116, 243)); // TSL+ : TSL
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