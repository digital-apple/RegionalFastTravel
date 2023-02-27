#pragma once

class Settings
{
public:
	[[nodiscard]] static Settings* GetSingleton();

	void SetupUniqueLocations();
	bool IsUniqueLocation(RE::FormID a_location);
	bool IsSameLocation(RE::BGSLocation* a_location, std::uint32_t a_id);
	bool GetQuestCompleted(std::uint32_t a_id);

	bool enableTravelHold{ false };
	bool enableTravelFaction{ false };
	bool enableTravelHabitation{ false };

	std::string notificationHold{ "Destiny is in a different hold." };
	std::string notificationFaction{ "You are not a member of the location's faction." };
	std::string notificationHabitation{ "Destiny is not an habitation." };

	std::string notificationSound{ "UISelectOff" };

	std::vector<RE::FormID> uniqueLocations;
private:
	const wchar_t* path{ L"Data/SKSE/Plugins/RegionalFastTravel.ini" };

	Settings();
};
