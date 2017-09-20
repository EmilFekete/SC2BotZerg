#include <sc2api/sc2_api.h>

#include <string>
#include <vector>
#include <iostream>

using namespace sc2;
using namespace std;

class Bot : public Agent {
public:
	virtual void OnGameStart() final {

	}

	virtual void OnStep() final {
		for (const auto &larva : larvas) {
			const Unit* unitP = getUnitWithTag(larva);
			if (unitP) {
				Debug()->DebugSphereOut(unitP->pos, 0.4f, Colors::Green);
			}
		}
		for (const auto &drone : drones) {
			const Unit* unitP = getUnitWithTag(drone);
			if (unitP) {
				Debug()->DebugSphereOut(unitP->pos, 0.5f, Colors::Red);
			}
		}
		Debug()->SendDebug();

		if (isOverlordNeeded() && larvas.size() > 0 && isResourcesAvailable(100)) {
			cout << "Training Overlord" << endl;
			Actions()->UnitCommand(larvas.back(), ABILITY_ID::TRAIN_OVERLORD);
			larvas.pop_back();
		}
		else if (larvas.size() > 0 && isResourcesAvailable(50) && !isOverlordNeeded()) {
			Actions()->UnitCommand(larvas.back(), ABILITY_ID::TRAIN_DRONE);
			larvas.pop_back();
			cout << "Training drone, larvas size: " << larvas.size() << endl;

		}
	}
	virtual void OnUnitCreated(const Unit& unit) final {
		auto myUnitsType = unit.unit_type.ToType();
		if (myUnitsType == UNIT_TYPEID::ZERG_LARVA) {
			cout << "larva created and added" << endl;
			larvas.push_back(*(Observation()->GetUnit(unit.tag)));
		}
		else if (myUnitsType == UNIT_TYPEID::ZERG_DRONE) {
			drones.push_back(*(Observation()->GetUnit(unit.tag)));
		}
	}

private:
	const Unit* getUnitWithTag(Tag unitTag) {
		return Observation()->GetUnit(unitTag);
	}
	bool isResourcesAvailable(int32_t mineralsNeeded, int32_t gasNeeded = 0) {
		int32_t mineralsCurrent = Observation()->GetMinerals();
		int32_t gasCurrent = Observation()->GetVespene();
		bool result = (mineralsCurrent - mineralsNeeded) >= 0 && (gasCurrent - gasNeeded) >= 0;
		return result;
	}
	bool isOverlordNeeded() const {
		return Observation()->GetFoodCap() <= Observation()->GetFoodUsed();
	}
	vector<Tag> larvas;
	vector<Tag> drones;
};

int main(int argc, char* argv[]) {
	Coordinator coordinator;
	coordinator.LoadSettings(argc, argv);

	Bot bot;
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