#pragma once
#include <ctime>
#include <iostream>
#include <string>
using namespace std;

class carInfo {
private:
	string carPlate;
	time_t enterTime;
	time_t outTime;
	
public:
	carInfo();
	carInfo(string carPlate, time_t startTime);
	~carInfo();
	string getCarPlate();
	time_t getEnterTime();
	time_t getOutTime();
	time_t getParkingTime();
	void setCarPlate(string carPlate);
	void setOutTime(time_t outTime);
	void setEnterTime(time_t enterTime);
	void initCarInfo();
	bool isValid();
};