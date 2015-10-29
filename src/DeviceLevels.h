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
