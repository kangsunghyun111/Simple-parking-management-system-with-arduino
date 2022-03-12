#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX

#include <opencv2/opencv.hpp>
#include <windows.h>
#include <iostream>
#include <cstdlib>
#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>
#include <regex>
#include <algorithm>
using namespace cv;
using namespace std;

void imageProcessing(string input);     // Image processing to extract strings form car plates  
string printCarNumber(string input);    // Print car number
string getCarNumber(string text);       // Use regular expression to get car number
char* UTF8ToANSI(const char* pszCode);  // To prevent korean broken