#include <Geode/Geode.hpp>
#include "include/PosDisplay.hpp"

#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;


class $modify(LevelInfoLayer) {
	struct Fields {
		PosDisplay* m_tslPos;
		PosDisplay* m_tslpPos;
	};

	$override bool init(GJGameLevel* level, bool challenge) {
		if (!LevelInfoLayer::init(level, challenge))
			return false;

		auto normalModeLabel = this -> getChildByID("normal-mode-label");

		auto tslPos = PosDisplay::create(PosDisplay::TSL);
		tslPos -> setID("tsl-position"_spr);
		auto tslpPos = PosDisplay::create(PosDisplay::TSLP);
		tslpPos -> setID("tslp-position"_spr);

		auto posMenu = CCMenu::create();
		posMenu -> setLayout(AnchorLayout::create());
		posMenu -> addChildAtPosition(
			tslPos,
			Anchor::Center,
			{ -(tslPos -> getContentWidth() + 37.f), 0.f }
		);
		posMenu -> addChildAtPosition(
			tslpPos,
			Anchor::Center,
			{ tslpPos -> getContentWidth() + 37.f, 0.f }
		);
		posMenu -> setID("positions"_spr);

		this -> addChild(posMenu);
		posMenu -> setPosition(
			normalModeLabel -> getPositionX(),
			normalModeLabel -> getPositionY() + 2.f
		);
		posMenu -> setZOrder(5);

		m_fields -> m_tslPos = tslPos;
		m_fields -> m_tslpPos = tslpPos;

		return true;
	}
};