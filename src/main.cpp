#include <Geode/Geode.hpp>

using namespace geode::prelude;


$on_mod(Loaded) {
	listenForSettingChangesV3("level-info-request-fail-notifs", [](bool) {
		Mod::get() -> setSavedValue("shown-notif-help", true);
	});
}

// placeholder is sincerely's government name.