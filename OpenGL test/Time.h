#pragma once

#include "windows.h"
#include "MasterRenderer.h"

class MasterRenderer;

class Time {
	
	friend MasterRenderer;

	DWORD LastTickCount;
	DWORD CurrentTickCount;
	Time() {
		LastTickCount = GetTickCount();
	}

public:
	static Time& getInstance() {
		static Time time;
		return time;
	}

	DWORD getCurrentTimeMS() {
		return GetTickCount();
	}

	DWORD getCurrentTimeS() {
		return getCurrentTimeMS()/1000.0;
	}

	float getTimePassedFromLastCallMS() {
		CurrentTickCount = GetTickCount();
		DWORD delta = CurrentTickCount - LastTickCount;
		Time::getInstance().LastTickCount = GetTickCount();

		return delta;
	}

	float getTimePassedFromLastCallS() {
		return getTimePassedFromLastCallMS() / 1000.0;
	}
};