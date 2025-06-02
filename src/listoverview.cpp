#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>

using namespace geode::prelude;


class $modify(HLevelSearchLayer, LevelSearchLayer) {
	$override bool init(int p0) {
		if (!LevelSearchLayer::init(p0))
			return false;

		auto sprite = CircleButtonSprite::createWithSpriteFrameName("tsl_logo.png"_spr);
		sprite -> setScale(0.8f);
		auto listOverviewButton = CCMenuItemSpriteExtra::create(
			sprite,
			this,
			menu_selector(HLevelSearchLayer::onListOverviewButton)
		);
		listOverviewButton -> setID("listOverviewButton"_spr);

		auto menu = static_cast<CCMenu*>(this -> getChildByID("other-filter-menu"));
		menu -> addChild(listOverviewButton);
		menu -> updateLayout();

		return true;
	}

	void onListOverviewButton(CCObject*) {


		return;
	}
};