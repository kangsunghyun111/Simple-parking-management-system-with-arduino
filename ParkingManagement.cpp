#include "ParkingManagement.h"

carInfo::carInfo() {
	this->carPlate = "";
	this->enterTime = -1;
	this->outTime = -1;
}

carInfo::carInfo(string carPlate, time_t startTime) {
	this->carPlate = carPlate;
	this->enterTime = startTime;
	this->outTime = -1;
}

carInfo::~carInfo() {
	cout << this->carPlate << " out\n";
}

string carInfo::getCarPlate() {
	return this->carPlate;
}

time_t carInfo::getEnterTime() {
	return this->enterTime;
}

time_t carInfo::getOutTime() {
	return this->outTime;
}

time_t carInfo::getParkingTime() {
	return (double)(this->outTime - this->enterTime);
}

void carInfo::setCarPlate(string carPlate) {
	this->carPlate = carPlate;
}

void carInfo::setEnterTime(time_t enterTime) {
	this->enterTime = enterTime;
}

void carInfo::setOutTime(time_t outTime) {
	this->outTime = outTime;
}

void carInfo::initCarInfo() {
	this->carPlate = "";
	this->enterTime = -1;
	this->outTime = -1;
}

bool carInfo::isValid() {
	if ((this->getParkingTime() > 5))
		return 1;
	else
		return 0;
}