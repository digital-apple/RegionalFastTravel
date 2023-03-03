#include "Hooks.h"
#include "Settings.h"

namespace Hooks
{
	bool func(RE::MapMenu* a_menu)
	{
		const auto marker = a_menu->mapMarker.get().get();

		if (marker) {
			const auto settings = Settings::GetSingleton();
			const auto currentLoc = RE::PlayerCharacter::GetSingleton()->GetCurrentLocation();
			const auto destinyLoc = marker->GetCurrentLocation();
			
			logger::info("* Hooks :: Latest marker ID: {:#x}", marker->GetFormID());

			if (const auto customMarker = settings->GetCustomMarker(marker->GetFormID()); customMarker.markerID != 0x0) {
				if (settings->CanFastTravel(currentLoc, customMarker.markerID) || settings->enableTravelHold) {
					if (customMarker.factionID != 0x0 || settings->enableTravelFaction) {
						if (RE::PlayerCharacter::GetSingleton()->IsInFaction(RE::TESForm::LookupByID<RE::TESFaction>(customMarker.factionID))) {
							return true;
						}
						settings->SendData(a_menu, "faction", true);
						return false;
					}

					if (customMarker.questID != 0x0 || settings->enableTravelFaction) {
						if (settings->GetQuestCompleted(customMarker.questID)) {
							return true;
						}
						settings->SendData(a_menu, "faction", true);
						return false;
					}
					return true;
				}
				settings->SendData(a_menu, "hold", true);
				return false;
			}

			if (!destinyLoc || !destinyLoc->parentLoc) {
				settings->SendData(a_menu, "data", false);
				return true;
			}

			if (settings->CanFastTravel(currentLoc, marker->GetFormID()) || settings->enableTravelHold) {
				if (settings->IsLocationAllowed(destinyLoc) || settings->enableTravelHabitation) {
					return true;
				}
				settings->SendData(a_menu, "habitation", true);
				return false;
			}
			settings->SendData(a_menu, "hold", true);
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
