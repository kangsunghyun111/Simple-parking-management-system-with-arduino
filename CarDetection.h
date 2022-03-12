#pragma once
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "ImageProcessing.h"
#include "ParkingManagement.h"
#include <iostream>
#include <stdio.h>
using namespace std;
using namespace cv;

void readVideoStream();
void carDetectAndDisplay(Mat frame);
int checkCarPlate(string carNumber);