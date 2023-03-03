#pragma once

class Settings
{
public:
	struct Marker
	{
		RE::FormID holdID{ 0x0 };
		RE::FormID questID{ 0x0 };
		RE::FormID markerID{ 0x0 };
		RE::FormID factionID{ 0x0 };
		RE::FormID locationID{ 0x0 };
	};

	[[nodiscard]] static Settings* GetSingleton();

	void ParseJson();
	void SetupKeywords();
	void SendData(RE::MapMenu* a_menu, std::string a_type, bool a_marker) const;

	std::string UpdateFormID(std::string a_formID) const;

	Marker GetCustomMarker(RE::FormID a_id) const; 
	Marker GetCustomLocation(RE::FormID a_id) const;
	
	bool IsLocationAllowed(RE::BGSLocation* a_location) const;
	bool GetQuestCompleted(RE::FormID a_id) const;
	bool CanFastTravel(RE::BGSLocation* a_location, RE::FormID a_marker);

	bool placeMarker{ false };
	bool enableTravelHold{ false };
	bool enableTravelFaction{ false };
	bool enableTravelHabitation{ false };
	bool disableNotificationQueue{ false };

	std::string notificationHold{ "Destiny is in a different hold or current location doesn't have a hold attached to it." };
	std::string notificationFaction{ "You are not a member of the location's faction." };
	std::string notificationHabitation{ "Destiny is not an habitation." };
	std::string notificationInsufficientData{ "Destiny doesn't have a location or hold attached to it." };

	std::string notificationSound{ "UISelectOff" };

	std::vector<Marker> customMarkers;
	std::vector<std::string> customKeywords;
private:
	const wchar_t* path{ L"Data/SKSE/Plugins/RegionalFastTravel.ini" };

	Settings();
};
