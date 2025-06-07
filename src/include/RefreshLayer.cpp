#include "RefreshLayer.hpp"


RefreshLayer* RefreshLayer::create(CCPoint const& refreshBtnPos) {
	auto ret = new RefreshLayer;
	if (ret -> init(refreshBtnPos)) {
		ret -> autorelease();
		return ret;
	}

	delete ret;
	return nullptr;
}

bool RefreshLayer::init(CCPoint const& refreshBtnPos) {
	if (!CCLayer::init())
		return false;

	this -> setOpacity(0);
	this -> runAction(CCFadeTo::create(0.25f, 200));
	this -> setID("RefreshLayer"_spr);

	m_layer = CCLayer::create();
	this -> addChild(m_layer);

	return true;
}