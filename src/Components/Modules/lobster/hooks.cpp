#include "Hooks.hpp"
#include "Functions.hpp"
#include <Components/Modules/Dvar.hpp>

#include <Components/Modules/GSC/Script.hpp>
#include <Utils/JSON.hpp>
#include <Components/Modules/Toast.hpp>

namespace Components::lobster
{
	Dvar::Var Instashoots;
	Dvar::Var AlwaysCanswap;
	Dvar::Var InstaMelees;

	void CheckForInstashoots(Game::pmove_s* pm)
	{
		if (pm->ps->weapCommon.weapon == 0)
			return;

		if (!Instashoots.get<bool>())
			return;

		if (!(pm->ps->weapState[0].weaponState == Game::WEAPON_RAISING || pm->ps->weapState[0].weaponState == Game::WEAPON_RAISING_ALTSWITCH))
			return;

		bool Dualwielding = Game::BG_GetEquippedWeaponState(pm->ps, pm->ps->weapCommon.weapon)->dualWielding;
		bool Instashoot = false;

		if (Dualwielding)
		{
			if ((pm->cmd.buttons & Game::CMD_BUTTON_ATTACK) || (pm->cmd.buttons & Game::CMD_BUTTON_THROW))
			{
				Instashoot = true;
			}
		}
		else
		{
			if (pm->cmd.buttons & Game::CMD_BUTTON_ATTACK)
			{
				Instashoot = true;
			}
		}

		if (Instashoot)
		{
			for (int i = 0; i < 2; i++)
			{
				pm->ps->weapState[i].weaponState = Game::WEAPON_READY;
				pm->ps->weapState[i].weaponTime = 0;
				pm->ps->weapState[i].weaponDelay = 0;
			}
		}
	}

	void CheckForAlwaysCanswap(Game::pmove_s* pm)
	{
		if (!AlwaysCanswap.get<bool>())
			return;

		for (int i = 0; i < 15; i++)
		{
			pm->ps->weapEquippedData[i].usedBefore = false;
		}
	}

	void CheckForInstaMelees(Game::pmove_s* pm)
	{
		if (!InstaMelees.get<bool>())
			return;

		if (pm->ps->weapCommon.weapon == 0)
			return;

		if (!(pm->ps->weapState[0].weaponState == Game::WEAPON_RAISING || pm->ps->weapState[0].weaponState == Game::WEAPON_RAISING_ALTSWITCH))
			return;

		if (pm->cmd.buttons & Game::CMD_BUTTON_MELEE)
		{
			for (int i = 0; i < 2; i++)
			{
				pm->ps->weapState[i].weaponState = Game::WEAPON_READY;
				pm->ps->weapState[i].weaponTime = 0;
				pm->ps->weapState[i].weaponDelay = 0;
			}
		}
	}


	void PM_WeaponStub(Game::pmove_s* pm, Game::pml_t* pml)
	{
		CheckForInstashoots(pm);
		CheckForAlwaysCanswap(pm);
		CheckForInstaMelees(pm);
		Functions::PM_Weapon(pm, pml);
	}

	int I_stricmpStub([[maybe_unused]] const char* a1, [[maybe_unused]] const char* a2)
	{
		return 0;
	}

	void addScriptMethods()
	{
		GSC::Script::AddMethod("setweapanim", [](Game::scr_entref_t entref)
			{
				const auto* ent = GSC::Script::Scr_GetPlayerEntity(entref);
				int anim = Game::Scr_GetInt(0);
				int hand = Game::Scr_GetInt(1);

				if (!anim)
				{
					Game::Scr_ParamError(0, "setweapanim: Illegal parameter(s)!");
					return;
				}
				if (hand > 2 && !hand)
				{
					Game::Scr_ParamError(1, "setweapanim: Illegal hand(s)!");
					return;
				}
				ent->client->ps.weapState[hand].weapAnim = anim;
			});

		GSC::Script::AddMethod("setweapstate", [](Game::scr_entref_t entref)
			{
				const auto* ent = GSC::Script::Scr_GetPlayerEntity(entref);
				int state = Game::Scr_GetInt(0);
				int hand = Game::Scr_GetInt(1);

				if (!state)
				{
					Game::Scr_ParamError(0, "setweapanim: Illegal state(s)!");
					return;
				}
				if (hand > 2 && !hand)
				{
					Game::Scr_ParamError(1, "setweapanim: Illegal hand(s)!");
					return;
				}
				ent->client->ps.weapState[hand].weaponState = state;
				
			});

		GSC::Script::AddMethod("resetweap", [](Game::scr_entref_t entref)
			{
				const auto* ent = GSC::Script::Scr_GetPlayerEntity(entref);
				for (int i = 0; i < 2; i++)
				{
					ent->client->ps.weapState[i].weaponState = Game::WEAPON_READY;
					ent->client->ps.weapState[i].weaponTime = 0;
					ent->client->ps.weapState[i].weaponDelay = 0;
				}
			});

		GSC::Script::AddFunction("Toast", [] //added command to gsc.
			{
				const auto* shader = Game::Scr_GetString(0);
				const auto* header = Game::Scr_GetString(1);
				const auto* desc = Game::Scr_GetString(2);
				int length = Game::Scr_GetInt(3);
				Components::Toast::Show(shader, header, desc, length);
			});

		GSC::Script::AddFunction("getLocalTime", []()
			{
				std::time_t now = std::time(nullptr);
				std::tm* localTime = std::localtime(&now);

				if (!localTime)
				{
					Game::Scr_AddString("time_error");
					return;
				}

				std::ostringstream oss;
				oss << std::put_time(localTime, "%H:%M:%S");

				std::string result = oss.str().substr(0, (1 << 16) - 1); // max 
				Game::Scr_AddString(result.c_str());
			});
	}

	Hooks::Hooks()
	{
		Instashoots = Dvar::Register<bool>("instashoots", false, Game::DVAR_SAVED, "Enables Instashoots");
		AlwaysCanswap = Dvar::Register<bool>("alwayscanswap", false, Game::DVAR_SAVED, "Always First Raise");
		InstaMelees = Dvar::Register<bool>("instamelees", false, Game::DVAR_SAVED, "Enables Instamelees");

		Utils::Hook(0x574960, PM_WeaponStub, HOOK_CALL).install()->quick();
		Utils::Hook(0x574AB2, PM_WeaponStub, HOOK_CALL).install()->quick();
		Utils::Hook(0x574B69, PM_WeaponStub, HOOK_CALL).install()->quick();


		//changes
		addScriptMethods();
		
	}

}
