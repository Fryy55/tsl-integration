#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;


class RefreshLayer final : public CCLayerColor {
public:
	static RefreshLayer* create(float ogX, float ogY);

private:
	bool init(float ogX, float ogY);
	void finalizeInit(float);
	void keyDown(enumKeyCodes) override;
	void helpPopup(float);
	void close();
	void finalizeClose(float);

	void onTSL(CCObject*);
	void onLevel(CCObject*);
	void onClose(CCObject*);

	~RefreshLayer() override;

	// Fields
	float m_ogX;
	float m_ogY;
	CCPoint m_tslMove;
	CCMenu* m_menu;
	CCMenu* m_closeMenu;
	CCLayer* m_layer;
	CCMenuItemSpriteExtra* m_levelRefreshBtn;
	CCMenuItemSpriteExtra* m_tslRefreshBtn;
	CCLabelBMFont* m_one;
	CCLabelBMFont* m_two;
	CCMenu* m_keysMenu;
	CCScale9Sprite* m_bg;
	bool m_closed;
};