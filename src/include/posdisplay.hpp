#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;


class PosDisplay : public CCMenu {
public:
	enum List {
		TSL = 0,
		TSLP = 1
	};

	static PosDisplay* create(List);

protected:
	bool init(List);

	// Fields
	CCSprite* m_icon;
	CCLabelBMFont* m_pos;
};