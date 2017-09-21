#pragma once
#include <sc2api/sc2_api.h>

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include "MapTools.h"

using namespace sc2;
using namespace std;

class ZergBot : public Agent {
public:
	ZergBot();
	virtual void OnGameStart() final;
	virtual void OnStep() final;
	virtual void OnUnitCreated(const Unit& unit) final;

private:
	const Unit* getUnitWithTag(Tag unitTag);
	bool isResourcesAvailable(int32_t mineralsNeeded, int32_t gasNeeded = 0);
	bool isOverlordNeeded() const;

	const size_t desiredDroneCount = 21;
	vector<Tag> larvas;
	vector<Tag> drones;
	size_t hatchingOverlords;
	MapTools mapTools;
};