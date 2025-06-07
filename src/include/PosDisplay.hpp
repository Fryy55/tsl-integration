#pragma once

#include <Geode/Geode.hpp>
#include "LevelInfo.hpp"

using namespace geode::prelude;


class PosDisplay final : public CCSprite {
public:
	enum List {
		TSL = 0,
		TSLP = 1
	};

	static PosDisplay* create(List, GJGameLevel* level, bool assumed);

	void onAssumedInfo(CCObject*);
	void onConcretePosition(CCObject*);

private:
	bool init(List, GJGameLevel*, bool);

	// Fields
	List m_list;
	CCMenu* m_posMenu;
	CCMenuItemSpriteExtra* m_posBtn;
	CCLabelBMFont* m_pos;
	GJGameLevel* m_level;
};