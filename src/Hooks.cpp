#include "Hooks.h"
#include "Settings.h"

namespace Hooks
{
	bool func(RE::MapMenu* a_menu)
	{
		const auto settings = Settings::GetSingleton();
		const auto reference = a_menu->mapMarker.get().get();

		if (reference) {
			const auto player = RE::PlayerCharacter::GetSingleton();
			const auto currentLocation = player->GetCurrentLocation();
			const auto destinyLocation = reference->GetCurrentLocation();
			
			if (reference->GetFormID() == 0x04016FE7) { // Eastmarch to Solstheim.
				if (currentLocation->parentLoc == RE::TESForm::LookupByID<RE::BGSLocation>(0x00018A57) || currentLocation->parentLoc == RE::TESForm::LookupByID<RE::BGSLocation>(0x0001676A) || settings->enableTravelHold) {
					return true;
				}
				RE::DebugNotification(settings->notificationHold.c_str(), settings->notificationSound.c_str());
				a_menu->PlaceMarker();
				return false;
			}

			if (reference->GetFormID() == 0x04016FE6) { // Solstheim to Eastmarch.
				if (currentLocation == RE::TESForm::LookupByID<RE::BGSLocation>(0x04016E2A) || currentLocation->parentLoc == RE::TESForm::LookupByID<RE::BGSLocation>(0x04016E2A)) {
					return true;
				}
				RE::DebugNotification(settings->notificationHold.c_str(), settings->notificationSound.c_str());
				a_menu->PlaceMarker();
				return false;
			}

			if (currentLocation->parentLoc == destinyLocation->parentLoc || currentLocation->parentLoc == destinyLocation->parentLoc->parentLoc || destinyLocation->parentLoc == RE::TESForm::LookupByID<RE::BGSLocation>(0x000130FF) || settings->enableTravelHold) {
				if (destinyLocation->HasKeywordString("LocTypeHabitation") || destinyLocation->HasKeywordString("LocTypeDwelling") || destinyLocation->HasKeywordString("LocTypeMilitaryCamp") || settings->IsUniqueLocation(destinyLocation->GetFormID()) || settings->enableTravelHabitation) {
					if (!settings->enableTravelFaction && destinyLocation->HasKeywordString("LocTypeMilitaryCamp") || !settings->enableTravelFaction && settings->IsUniqueLocation(destinyLocation->GetFormID())) {
						if (settings->IsSameLocation(destinyLocation, 0x00076F3A) && settings->GetQuestCompleted(0x0001F251)) { return true; } // College of Winterhold | "First Lessons"
						if (settings->IsSameLocation(destinyLocation, 0x0003444E) && settings->GetQuestCompleted(0x0001EA51)) { return true; } // Dark Brotherhood Sanctuary | "With Friends Like These..."
						if (settings->IsSameLocation(destinyLocation, 0x00019429) && settings->GetQuestCompleted(0x0001EA59)) { return true; } // Dawnstar Sanctuary | "Hail Sithis!"
						if (settings->IsSameLocation(destinyLocation, 0x00018E34) && settings->GetQuestCompleted(0x000242BA)) { return true; } // High Hrothgar | "The Way Of The Voice"
						if (settings->IsSameLocation(destinyLocation, 0x00018E42) && settings->GetQuestCompleted(0x00036192)) { return true; } // Sky Haven Temple | "Alduin's Wall"

						if (settings->IsSameLocation(destinyLocation, 0x0001916F) && player->IsInFaction(RE::TESForm::LookupByID<RE::TESFaction>(0x000E3609))) { return true; } // Druadach Redoubt | "No One Escapes Cidhna Mine"

						if (destinyLocation->HasKeywordString("LocTypeOrcStronghold") && player->IsInFaction(RE::TESForm::LookupByID<RE::TESFaction>(0x00024029))) { return true; } // Orc Stronghold | "The Forgemaster's Fingers" OR being an Orsimer.
						if (destinyLocation->HasKeywordString("CWCampImperial") && settings->GetQuestCompleted(0x000D517A)) { return true; }	// Imperial Military Camp | "Joining the Legion"
						if (destinyLocation->HasKeywordString("CWCampSons") && settings->GetQuestCompleted(0x000E2D29)) { return true; }		// Sons of Skyrim Military Camp | "Joining the Stormcloaks"

						if (currentLocation->parentLoc == RE::TESForm::LookupByID<RE::BGSLocation>(0x00018A58) || currentLocation->parentLoc == RE::TESForm::LookupByID<RE::BGSLocation>(0x0001676C)) { // RiftenLocation OR RiftHoldLocation.
							if (settings->IsSameLocation(destinyLocation, 0x02004C1F) && settings->GetQuestCompleted(0x02004E24)) { return true; } // Dayspring Canyon | "A New Order"
						}

						if (currentLocation->parentLoc == RE::TESForm::LookupByID<RE::BGSLocation>(0x00018A5A) || currentLocation->parentLoc == RE::TESForm::LookupByID<RE::BGSLocation>(0x00016770)) { // SolitudeLocation OR HaafingarHoldLocation.
							if (settings->IsSameLocation(destinyLocation, 0x02004C20) && settings->GetQuestCompleted(0x0200594C)) { return true; } // Castle Volkihar | "The Bloodstone Chalice"
						}

						RE::DebugNotification(settings->notificationFaction.c_str(), settings->notificationSound.c_str());
						a_menu->PlaceMarker();
						return false;
					}
					return true;
				}
				RE::DebugNotification(settings->notificationHabitation.c_str(), settings->notificationSound.c_str());
				a_menu->PlaceMarker();
				return false;
			}
			RE::DebugNotification(settings->notificationHold.c_str(), settings->notificationSound.c_str());
			a_menu->PlaceMarker();
			return false;
		}
		return false;
	}

	void Install()
	{
		REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(52208, 53095), OFFSET(0x2D2, 0x335) };

		struct Patch : Xbyak::CodeGenerator
		{
			Patch(std::uintptr_t a_func, std::uintptr_t a_addr, std::uintptr_t a_stop)
			{
				Xbyak::Label funcLabel;
				Xbyak::Label retnLabel;
				Xbyak::Label skipLabel;
				Xbyak::Label stopLabel;

#ifdef SKYRIM_AE
				mov(dword[r14 + 0x58], ebx);
				mov(dword[rbp - 0x59], r13);
				mov(dword[rbp - 0x51], r13d);

				mov(rcx, r14);
#else
				mov(dword[rsi + 0x58], ebx);
				mov(dword[rbp - 0x41], r13);
				mov(dword[rbp - 0x39], r13d);

				mov(rcx, rsi);
#endif

				sub(rsp, 0x20);
				call(ptr[rip + funcLabel]);
				add(rsp, 0x20);
				test(al, al);
				jz(skipLabel);
				
				jmp(ptr[rip + retnLabel]);

				L(skipLabel);
				jmp(ptr[rip + stopLabel]);

				L(retnLabel);
				dq(a_addr + OFFSET(0xB, 0xC));

				L(stopLabel);
				dq(a_stop);

				L(funcLabel);
				dq(a_func);
			}
		};

		Patch patch(reinterpret_cast<uintptr_t>(func), target.address(), target.address() + OFFSET(0x24A, 0x1D9));
		patch.ready();

		auto& trampoline = SKSE::GetTrampoline();
		SKSE::AllocTrampoline(83);

		trampoline.write_branch<5>(target.address(), trampoline.allocate(patch));

		logger::info("* Hooks :: Installed Hook.\n");
	}
}
