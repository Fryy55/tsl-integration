// this is my most hated TU

#include "RefreshLayer.hpp"
#include "LevelInfo.hpp"
#include "ContentTo.hpp"


RefreshLayer* RefreshLayer::create(float ogX, float ogY) {
	auto ret = new RefreshLayer;
	if (ret -> init(ogX, ogY)) {
		ret -> autorelease();
		return ret;
	}

	delete ret;
	return nullptr;
}

bool RefreshLayer::init(float ogX, float ogY) {
	if (!CCLayerColor::initWithColor(ccc4(0, 0, 0, 0)))
		return false;

	// i wish i didn't have to do this... (i'm NOT inheriting geode::popup/flalertlayer :middlefinger:)
	CCTouchDispatcher::get() -> registerForcePrio(this, 2);
	CCTouchDispatcher::get() -> addTargetedDelegate(this, this -> getTouchPriority(), true);
	this -> setTouchEnabled(true);
	this -> setKeypadEnabled(true);

	this -> setID("RefreshLayer"_spr);

	m_closed = false;
	m_ogX = ogX;
	m_ogY = ogY;
	m_tslMove = ccp(-50.f, 0.f);
	m_layer = CCLayer::create();
	m_layer -> setID("main-layer"_spr);
	this -> addChild(m_layer);
	m_menu = CCMenu::create();
	m_menu -> setID("main-menu"_spr);
	m_layer -> addChildAtPosition(m_menu, Anchor::Center);
	m_menu -> setTouchPriority(-503); // :pensive:
	m_closeMenu = CCMenu::create();
	m_closeMenu -> setAnchorPoint({ 0.f, 0.f });
	m_closeMenu -> setID("close-menu"_spr);
	m_layer -> addChild(m_closeMenu);

	auto winsize = CCDirector::get() -> getWinSize();
	// @geode-ignore(unknown-resource) SHUT UP!!!!!!!!!!!!!!
	m_bg = CCScale9Sprite::createWithSpriteFrameName("geode.loader/category-bg.png");
	m_bg -> setScale(1.5f);
	m_bg -> setAnchorPoint({ 1.f, 0.5f });
	m_bg -> setPosition(winsize.width, m_ogY);
	m_bg -> setContentWidth(40.f);
	auto bgColorRGBA = Mod::get() -> getSettingValue<ccColor4B>("refresh-layer-bg-color");
	m_bg -> setColor({ bgColorRGBA.r, bgColorRGBA.g, bgColorRGBA.b });
	m_bg -> setID("bg"_spr);
	m_menu -> addChild(m_bg);
	m_levelRefreshBtn = CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png"),
		this,
		menu_selector(RefreshLayer::onLevel)
	);
	m_levelRefreshBtn -> setPosition(m_ogX, m_ogY);
	m_levelRefreshBtn -> setUserObject("orig"_spr, CCBool::create(true));
	m_levelRefreshBtn -> setID("level-refresh-button"_spr);
	m_menu -> addChild(m_levelRefreshBtn, 2);
	auto tslRefreshBtnSpr = CircleButtonSprite::create(
		CCSprite::createWithSpriteFrameName("placeholder.png"_spr)
	);
	tslRefreshBtnSpr -> setID("tsl-refresh-button-sprite"_spr);
	m_tslRefreshBtn = CCMenuItemSpriteExtra::create(
		tslRefreshBtnSpr,
		this,
		menu_selector(RefreshLayer::onTSL)
	);
	m_tslRefreshBtn -> setPosition(m_ogX, m_ogY);
	m_tslRefreshBtn -> setID("tsl-refresh-button"_spr);
	m_menu -> addChild(m_tslRefreshBtn, 1);
	auto closeBtnSpr = CCSprite::createWithSpriteFrameName("diffIcon_02_btn_001.png");
	closeBtnSpr -> setScaleX(winsize.width / closeBtnSpr -> getContentWidth());
	closeBtnSpr -> setScaleY(winsize.height / closeBtnSpr -> getContentHeight());
	closeBtnSpr -> setVisible(false);
	closeBtnSpr -> setID("close-button-sprite"_spr);
	auto closeBtn = CCMenuItemSpriteExtra::create(
		closeBtnSpr,
		this,
		menu_selector(RefreshLayer::onClose)
	);
	closeBtn -> setID("close-button"_spr);
	m_closeMenu -> addChildAtPosition(closeBtn, Anchor::Center);

	#ifdef GEODE_IS_DESKTOP
		m_keysMenu = CCMenu::create();
		m_keysMenu -> setCascadeOpacityEnabled(true);
		m_keysMenu -> setID("keys-menu"_spr);
		m_layer -> addChild(m_keysMenu, 5);
		m_one = CCLabelBMFont::create("1", "bigFont.fnt");
		m_one -> setScale(0.8f);
		m_one -> setID("one"_spr);
		m_keysMenu -> addChild(m_one);
		m_one -> setPosition(
			m_tslRefreshBtn -> getPositionX() + m_tslMove.x + 15.f,
			m_tslRefreshBtn -> getPositionY() + m_tslMove.y - 15.f
		);
		m_two = CCLabelBMFont::create("2", "bigFont.fnt");
		m_two -> setScale(0.8f);
		m_two -> setID("two"_spr);
		m_keysMenu -> addChild(m_two);
		m_two -> setPosition(
			m_levelRefreshBtn -> getPositionX() + 15.f,
			m_levelRefreshBtn -> getPositionY() - 15.f
		);
		m_keysMenu -> setOpacity(0);
		m_keysMenu -> runAction(
			CCSequence::createWithTwoActions(CCDelayTime::create(0.4f), CCFadeIn::create(0.15f))
		);
	#endif

	m_bg -> runAction(CCEaseInOut::create(ContentTo::create(0.25, 73.f), 4.f));
	m_bg -> setOpacity(0);
	m_bg -> runAction(CCFadeTo::create(0.25f, bgColorRGBA.a));
	m_tslRefreshBtn -> setEnabled(false);
	m_tslRefreshBtn -> runAction(CCEaseInOut::create(CCMoveBy::create(0.25f, m_tslMove), 4.f));
	this -> scheduleOnce(schedule_selector(RefreshLayer::finalizeInit), 0.25f);

	this -> runAction(CCFadeTo::create(0.25f, 215));

	if (!Mod::get() -> setSavedValue("shown-refresh-menu-help", true))
		this -> scheduleOnce(schedule_selector(RefreshLayer::helpPopup), 0.f); // :silly:

	return true;
}

void RefreshLayer::finalizeInit(float) {
	m_tslRefreshBtn -> setEnabled(true);

	return;
}

void RefreshLayer::keyDown(enumKeyCodes key) {
	switch(key) {
		case KEY_Escape:
			this -> close();
			break;
		case KEY_One:
			this -> onTSL(nullptr);
			break;
		case KEY_Two:
			this -> onLevel(m_levelRefreshBtn);
			break;
		default:
			break;
	}

	return;
}

void RefreshLayer::helpPopup(float) {
	#ifdef GEODE_IS_DESKTOP
		FLAlertLayer::create(
			"Refresh menu",
			"Hello from <cb>TSL Integration</c>!\nThis is what the new <cc>refresh menu</c> looks like! "
			"If you ever need to manually refresh <cl>TSL</c>, you can do it with the first button"
			"/by pressing <cg>1</c> on your keyboard"
			"; refreshing the <cd>level</c> is now done with the second button"
			"/by pressing <cg>2</c>"
			".\nYou can customize the background color in mod's <cy>settings</c>.",
			"OK"
		) -> show();
	#else
		FLAlertLayer::create(
			"Refresh menu",
			"Hello from <cb>TSL Integration</c>!\nThis is what the new <cc>refresh menu</c> looks like! "
			"If you ever need to manually refresh <cl>TSL</c>, you can do it with the first button"
			"; refreshing the <cd>level</c> is now done with the second button"
			".\nYou can customize the background color in mod's <cy>settings</c>.",
			"OK"
		) -> show();
	#endif

	return;
}

void RefreshLayer::close() {
	m_closed = true;

	CCTouchDispatcher::get() -> unregisterForcePrio(this);
	this -> setTouchEnabled(false);
	this -> setKeypadEnabled(false);
	m_menu -> setTouchEnabled(false);
	m_closeMenu -> setTouchEnabled(false);
	m_levelRefreshBtn -> removeFromParent();
	m_tslRefreshBtn -> unselected();

	#ifdef GEODE_IS_DESKTOP
		m_keysMenu -> stopAllActions();
		m_keysMenu -> runAction(CCFadeTo::create(0.1f, 0));
	#endif
	m_bg -> stopAllActions();
	m_bg -> runAction(CCEaseInOut::create(ContentTo::create(0.25, 40.f), 4.f));
	m_bg -> runAction(CCFadeTo::create(0.2f, 0));
	m_tslRefreshBtn -> runAction(CCEaseInOut::create(CCMoveBy::create(0.25f, -m_tslMove), 4.f));
	m_tslRefreshBtn -> runAction(CCFadeOut::create(0.25f));
	this -> runAction(CCFadeTo::create(0.25f, 0));

	auto tempMenu = CCMenu::create();
	tempMenu -> setID("temp-menu"_spr);
	auto coverupButton = CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png"),
		this -> getParent() -> getChildByID("LevelInfoLayer"),
		menu_selector(LevelInfoLayer::onUpdate)
	);
	coverupButton -> setID("coverup-button"_spr);
	m_layer -> addChildAtPosition(tempMenu, Anchor::Center);
	tempMenu -> addChild(coverupButton);
	coverupButton -> setPosition(m_ogX, m_ogY);
	this -> scheduleOnce(schedule_selector(RefreshLayer::finalizeClose), 0.25f);

	return;
}

void RefreshLayer::finalizeClose(float) {
	this -> removeFromParent();

	return;
}

void RefreshLayer::onTSL(CCObject*) {
	LevelInfo::get() -> manualRefresh();

	this -> close();

	return;
}

void RefreshLayer::onLevel(CCObject* sender) {
	static_cast<LevelInfoLayer*>(
		this -> m_pParent -> getChildByID("LevelInfoLayer")
	) -> onUpdate(sender);

	this -> close();

	return;
}

void RefreshLayer::onClose(CCObject*) {
	this -> close();

	return;
}

RefreshLayer::~RefreshLayer() {
	if (!m_closed) {
		CCTouchDispatcher::get() -> unregisterForcePrio(this);
		this -> setTouchEnabled(false);
		log::info("ik i shouldn't really log stuff like that but istg, the thing that just got caught is "
		"a super obscure bug that has to do with retains for touch prio i think but it's so hilariously "
		"rare and unexplained that i just HAVE to log it okay?");
	}
}