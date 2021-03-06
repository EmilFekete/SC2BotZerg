#include <sc2api/sc2_api.h>

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include "ZergBot.h"

using namespace sc2;
using namespace std;

int main(int argc, char* argv[]) {
	Coordinator coordinator;
	coordinator.LoadSettings(argc, argv);

	ZergBot bot;
	coordinator.SetParticipants({
		CreateParticipant(Race::Zerg, &bot),
		CreateComputer(Race::Terran)
	});

	coordinator.LaunchStarcraft();
	coordinator.StartGame(sc2::kMapBelShirVestigeLE);

	while (coordinator.Update()) {
	}

	return 0;
}