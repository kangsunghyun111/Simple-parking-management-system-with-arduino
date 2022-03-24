#pragma once
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "ImageProcessing.h"
#include "ParkingManagement.h"
#include "SerialClass.h"
#include <iostream>
#include <stdio.h>
using namespace std;
using namespace cv;

void readVideoStream();	// Read video
void carDetectAndDisplay(Mat frame, Serial* SP);	// Detcet car lisence plate and process image
int checkCarPlate(string carNumber);	// If the lisence plate is in the car list, return index. Else return -1