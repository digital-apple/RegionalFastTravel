#include "Settings.h"

Settings* Settings::GetSingleton()
{
	static Settings singleton;
	return std::addressof(singleton);
}

Settings::Settings()
{
	CSimpleIniA ini;
	ini.SetUnicode();

	ini.LoadFile(path);

	ini::get_value(ini, enableTravelHold, "Settings", "Marker Placement", "; Places the player's custom marker to the marker you weren't able to travel to.");
	ini::get_value(ini, enableTravelHold, "Settings", "Hold Fast Travel", "; Allows fast travel between different holds.");
	ini::get_value(ini, enableTravelFaction, "Settings", "Faction Fast Travel", "; Allows fast travel to locations owned by factions while not being a member of them.");
	ini::get_value(ini, enableTravelHabitation, "Settings", "Habitation Fast Travel", "; Allows fast travel to locations that doesn't have the permitted keywords.");
	ini::get_value(ini, disableNotificationQueue, "Settings", "Notification Queue", "; This setting will allow notifications to be displayed even if they are already in queue.");

	ini::get_value(ini, notificationHold, "Notifications", "Hold Notification", "; The notification that will be displayed when you can't fast travel to a location due to it being in a different hold.");
	ini::get_value(ini, notificationFaction, "Notifications", "Faction Notification", "; The notification that will be displayed when you can't fast travel to a location due to you not being a location's faction member.");
	ini::get_value(ini, notificationHabitation, "Notifications", "Habitation Notification", "; The notification that will be displayed when you can't fast travel due to the location not being a habitation.");
	ini::get_value(ini, notificationInsufficientData, "Notifications", "Insufficient Data Notification", "; The notification that will be displayed when the map marker doesn't have any data assigned to it. This has be done manually for non-vanilla map markers.");

	ini::get_value(ini, notificationSound, "Sound", "Notification Sound", "; The editor ID of the sound that will be played if you can't fast travel to a location.");

	(void)ini.SaveFile(path);
}

void Settings::SetupCustomMarkers()
{
	CSimpleIniA ini;
	ini.SetUnicode();
	ini.LoadFile(path);

	if (const auto section = ini.GetSection("Custom Markers"); section && !section->empty()) {
		for (const auto& entry : *section | std::views::values) {
			std::vector<std::string> parts;

			for (auto sub : std::string((entry)) | std::views::split('|')) {
				parts.push_back(std::string(sub.begin(), sub.end()));
			}

			Settings::Marker customMarker{ stoul(parts[0], 0, 16), stoul(parts[1], 0, 16), stoul(parts[2], 0, 16), stoul(parts[3], 0, 16) };

			customMarkers.emplace_back(customMarker);
		}
		logger::info("* Settings :: Done setting up {} custom markers.", customMarkers.size());
	}
}

void Settings::SetupCustomKeywords()
{
	CSimpleIniA ini;
	ini.SetUnicode();
	ini.LoadFile(path);

	if (const auto section = ini.GetSection("Custom Keywords"); section && !section->empty()) {
		for (const auto& entry : *section | std::views::values) {
			std::vector<std::string> parts;

			for (auto sub : std::string((entry)) | std::views::split('|')) {
				parts.push_back(std::string(sub.begin(), sub.end()));
			}

			for (auto& part : parts) {
				if (!part.empty()) {
					customKeywords.emplace_back(part);
				}
			}
		}
		logger::info("* Settings :: Done setting up {} custom keywords.", customKeywords.size());
	}
}

Settings::Marker Settings::IsCustomMarker(RE::FormID a_id) const
{
	for (auto& customMarker : customMarkers) {
		if (customMarker.markerID == a_id) {
			logger::info("* Settings :: Custom Marker {:#x} detected, applying custom rules...", a_id);
			return customMarker;
		}
	}
	return Settings::Marker{0x0, 0x0, 0x0, 0x0};
}

bool Settings::IsLocationAllowed(RE::BGSLocation* a_location) const
{
	if (a_location) {
		for (auto& customKeyword : customKeywords) {
			if (a_location->HasKeywordString(customKeyword)) {
				return true;
			}
		}
	}
	return false;
}

bool Settings::GetQuestCompleted(RE::FormID a_id) const
{
	const auto quest = RE::TESForm::LookupByID<RE::TESQuest>(a_id);

	if (quest) {
		if (quest->IsCompleted()) {
			return true;
		}
	}
	return false;
}

void Settings::SendData(RE::MapMenu* a_menu, std::string a_type, bool a_marker) const
{
	if (a_menu) {
		if (a_type == "hold") {
			RE::DebugNotification(Settings::notificationHold.c_str(), Settings::notificationSound.c_str(), Settings::disableNotificationQueue);
		}
		else if (a_type == "habitation") {
			RE::DebugNotification(Settings::notificationHabitation.c_str(), Settings::notificationSound.c_str(), Settings::disableNotificationQueue);
		}
		else if (a_type == "faction") {
			RE::DebugNotification(Settings::notificationFaction.c_str(), Settings::notificationSound.c_str(), Settings::disableNotificationQueue);
		}
		else {
			RE::DebugNotification(Settings::notificationInsufficientData.c_str(), Settings::notificationSound.c_str(), Settings::disableNotificationQueue);
		}

		if (Settings::placeMarker && a_marker) {
			a_menu->PlaceMarker();
		}
	}
}
