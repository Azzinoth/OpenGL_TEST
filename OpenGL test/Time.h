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

	DWORD delta;

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

	void startNewFrame() {
		CurrentTickCount = GetTickCount();
		Time::getInstance().delta = CurrentTickCount - LastTickCount;
		Time::getInstance().LastTickCount = GetTickCount();
	}

	float getTimePassedFromLastCallMS() {
		return Time::getInstance().delta;
	}

	float getTimePassedFromLastCallS() {
		return getTimePassedFromLastCallMS() / 1000.0;
	}
};