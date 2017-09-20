#include <sc2api/sc2_api.h>

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace sc2;
using namespace std;

class Bot : public Agent {
public:
	virtual void OnGameStart() final {
		hatchingOverlords = 1;
		cout << "overlord count: " << hatchingOverlords << endl;
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

		// if we morphed an egg, remove it from the larvas
		Units eggs = Observation()->GetUnits(Unit::Alliance::Self, [](const Unit& u) {return u.unit_type.ToType() == UNIT_TYPEID::ZERG_EGG; });
		for (const auto &egg : eggs) {
			larvas.erase(std::remove(larvas.begin(), larvas.end(), egg.tag), larvas.end());
		}

		if (isOverlordNeeded() && larvas.size() > 0 && isResourcesAvailable(100)) {
			hatchingOverlords++;
			cout << "hatching overlords" << endl;
			Actions()->UnitCommand(larvas.back(), ABILITY_ID::TRAIN_OVERLORD);
		} else if (larvas.size() > 0 && isResourcesAvailable(50) && drones.size() < desiredDroneCount) {
			Actions()->UnitCommand(larvas.back(), ABILITY_ID::TRAIN_DRONE);
		}
	}

	virtual void OnUnitCreated(const Unit& unit) final {
		auto myUnitsType = unit.unit_type.ToType();
		if (myUnitsType == UNIT_TYPEID::ZERG_LARVA) {
			larvas.push_back(*(Observation()->GetUnit(unit.tag)));
		}
		else if (myUnitsType == UNIT_TYPEID::ZERG_DRONE) {
			drones.push_back(*(Observation()->GetUnit(unit.tag)));
		} else if (myUnitsType == UNIT_TYPEID::ZERG_OVERLORD) {
			hatchingOverlords--;
		}
	}

private:
	const size_t desiredDroneCount = 21;
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
		cout << (Observation()->GetFoodCap() <= Observation()->GetFoodUsed() && hatchingOverlords < 1) << endl;
		return (Observation()->GetFoodCap() <= Observation()->GetFoodUsed() && hatchingOverlords < 1);
	}

	vector<Tag> larvas;
	vector<Tag> drones;
	size_t hatchingOverlords;
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