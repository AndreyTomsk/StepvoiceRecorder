/*
Stepvoice Recorder
Copyright (C) 2004-2016 Andrey Firsov
*/

#pragma once
#include "WasapiHelpers.h"
#include <vector>

class CDeviceLevel; //forward
class CDeviceLevels
{
public:
	CDeviceLevels(const WasapiHelpers::DevicesArray& devices);
	~CDeviceLevels();
	float GetLevel(int deviceID) const;

private:
	std::vector<CDeviceLevel*> m_deviceLevels;
};
