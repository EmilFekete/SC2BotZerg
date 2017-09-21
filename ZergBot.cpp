#include "ZergBot.h"

ZergBot::ZergBot() : mapTools(this) {}

void ZergBot::OnGameStart() {
	hatchingOverlords = 1;
}

void ZergBot::OnStep() {
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
		Actions()->UnitCommand(larvas.back(), ABILITY_ID::TRAIN_OVERLORD);
	}
	else if (larvas.size() > 0 && isResourcesAvailable(50) && drones.size() < desiredDroneCount) {
		Actions()->UnitCommand(larvas.back(), ABILITY_ID::TRAIN_DRONE);
	}
}

void ZergBot::OnUnitCreated(const Unit& unit) {
	auto myUnitsType = unit.unit_type.ToType();
	if (myUnitsType == UNIT_TYPEID::ZERG_LARVA) {
		larvas.push_back(*(Observation()->GetUnit(unit.tag)));
	}
	else if (myUnitsType == UNIT_TYPEID::ZERG_DRONE) {
		drones.push_back(*(Observation()->GetUnit(unit.tag)));
	}
	else if (myUnitsType == UNIT_TYPEID::ZERG_OVERLORD) {
		hatchingOverlords--;
	}
}

const Unit* ZergBot::getUnitWithTag(Tag unitTag) {
	return Observation()->GetUnit(unitTag);
}
bool ZergBot::isResourcesAvailable(int32_t mineralsNeeded, int32_t gasNeeded) {
	int32_t mineralsCurrent = Observation()->GetMinerals();
	int32_t gasCurrent = Observation()->GetVespene();
	bool result = (mineralsCurrent - mineralsNeeded) >= 0 && (gasCurrent - gasNeeded) >= 0;
	return result;
}

bool ZergBot::isOverlordNeeded() const {
	return (Observation()->GetFoodCap() <= Observation()->GetFoodUsed() && hatchingOverlords < 1);
}