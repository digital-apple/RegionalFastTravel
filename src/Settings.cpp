#include "Settings.h"

Settings* Settings::GetSingleton()
{
	static Settings singleton;
	return std::addressof(singleton);
}

Settings::Settings()
{
	SetupUniqueLocations();

	CSimpleIniA ini;
	ini.SetUnicode();

	ini.LoadFile(path);

	ini::get_value(ini, enableTravelHold, "Settings", "Hold Fast Travel", "; Allows fast travel between different holds.");
	ini::get_value(ini, enableTravelFaction, "Settings", "Faction Fast Travel", "; Allows fast travel to locations owned by factions while not being a member of them.");
	ini::get_value(ini, enableTravelHabitation, "Settings", "Habitation Fast Travel", "; Allows fast travel to locations that doesn't have the 'LocTypeHabitation' or 'LocTypeDwelling' keywords.");

	ini::get_value(ini, notificationHold, "Notifications", "Hold Notification", "; The notification that will be displayed when you can't fast travel to a location due to it being in a different hold.");
	ini::get_value(ini, notificationFaction, "Notifications", "Faction Notification", "; The notification that will be displayed when you can't fast travel to a location due to you not being a location's faction member.");
	ini::get_value(ini, notificationHabitation, "Notifications", "Habitation Notification", "; The notification that will be displayed when you can't fast travel due to the location not being a habitation.");

	ini::get_value(ini, notificationSound, "Sound", "Notification Sound", "; The editor ID of the sound that will be played if you can't fast travel to a location.");

	(void)ini.SaveFile(path);
}

void Settings::SetupUniqueLocations()
{
	uniqueLocations.push_back(static_cast<RE::FormID>(0x00076F3A));
	uniqueLocations.push_back(static_cast<RE::FormID>(0x0003444E));
	uniqueLocations.push_back(static_cast<RE::FormID>(0x00019429));
	uniqueLocations.push_back(static_cast<RE::FormID>(0x00018E34));
	uniqueLocations.push_back(static_cast<RE::FormID>(0x00018E42));
	uniqueLocations.push_back(static_cast<RE::FormID>(0x0001916F));
	uniqueLocations.push_back(static_cast<RE::FormID>(0x02004C1F)); 
	uniqueLocations.push_back(static_cast<RE::FormID>(0x02004C20));

	logger::info("* Settings :: Done setting up unique locations.");
}

bool Settings::IsUniqueLocation(RE::FormID a_location)
{
	if (a_location) {
		for (std::uint32_t i = 0; i < uniqueLocations.size(); i++) {
			if (uniqueLocations[i] == a_location) {
				return true;
			}
		}
	}
	return false;
}

bool Settings::IsSameLocation(RE::BGSLocation* a_location, std::uint32_t a_id)
{
	if (a_location) {
		const auto sourceID = a_location->GetFormID();
		const auto targetID = static_cast<RE::FormID>(a_id);

		if (sourceID == targetID) {
			return true;
		}
	}
	return false;
}

bool Settings::GetQuestCompleted(std::uint32_t a_id)
{
	const auto quest = RE::TESForm::LookupByID<RE::TESQuest>(a_id);

	if (quest) {
		if (quest->IsCompleted()) {
			return true;
		}
	}
	return false;
}
