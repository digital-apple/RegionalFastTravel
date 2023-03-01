#pragma once

class Settings
{
public:
	struct Marker
	{
		RE::FormID markerID{ 0x0 };
		RE::FormID holdID{ 0x0 };
		RE::FormID factionID{ 0x0 };
		RE::FormID questID{ 0x0 };
	};

	[[nodiscard]] static Settings* GetSingleton();

	void SetupCustomMarkers();
	void SetupCustomKeywords();

	Marker IsCustomMarker(RE::FormID a_id) const; 

	bool IsLocationAllowed(RE::BGSLocation* a_location) const;
	bool GetQuestCompleted(RE::FormID a_id) const;

	void SendData(RE::MapMenu* a_menu, std::string a_type, bool a_marker) const;

	
	bool placeMarker{ false };
	bool enableTravelHold{ false };
	bool enableTravelFaction{ false };
	bool enableTravelHabitation{ false };
	bool disableNotificationQueue{ false };

	std::string notificationHold{ "Destiny is in a different hold." };
	std::string notificationFaction{ "You are not a member of the location's faction." };
	std::string notificationHabitation{ "Destiny is not an habitation." };
	std::string notificationInsufficientData{ "Destiny doesn't have sufficient data assigned to it." };

	std::string notificationSound{ "UISelectOff" };

	std::vector<std::string> customKeywords;
	std::vector<Marker> customMarkers;
private:
	const wchar_t* path{ L"Data/SKSE/Plugins/RegionalFastTravel.ini" };

	Settings();
};
