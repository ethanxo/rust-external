#pragma once

namespace globals {
	namespace general {
		bool menu_open = true;
	}

	namespace aimbot {
		bool enabled = true;
		bool autoshoot = false;
		bool silent = false;

		int key = 0x06; // m5 i think?

		float fov = 45.f;
	}

	namespace visuals {
		// Entities
		bool enabled_players = true;
		bool enabled_animals = false;
		bool enabled_vehicles = false;
		bool enabled_ores = false;
		bool enabled_traps = true;
		bool enabled_pickups = false;
		bool enabled_items = true;
		bool enabled_npc = false;
		bool enabled_stash = true;

		// Features
		bool box = true;
		bool healthbar = true;
		bool name = true;
		bool distance = false;
		bool skeleton = true;

		// Extras
		float fontsize = 10.f;
		float maxdistance = 500.f;
	}

	namespace misc {
		bool crosshair = true;
		bool admin = true;
		bool thickbullet = false;

		float recoilscale = 1.f;
		float spreadscale = 1.f;
	}
}