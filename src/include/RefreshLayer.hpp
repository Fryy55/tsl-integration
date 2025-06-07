#include <Geode/Geode.hpp>

using namespace geode::prelude;


class RefreshLayer final : public CCLayerRGBA {
public:
	static RefreshLayer* create(CCPoint const& refreshBtnPos);

private:
	bool init(CCPoint const& refreshBtnPos);

	// Fields
	CCLayer* m_layer;
};