#include <stdio.h>
#include <tchar.h>
#include "SerialClass.h"	// Library described above
#include "CarDetection.h"
#include <string>

// application reads from the specified serial port and reports the collected data
int _tmain(int argc, _TCHAR* argv[])
{	
	readVideoStream();


	/* For serial communications test */
	//printf("Welcome to the serial test app!\n\n");

	//Serial* SP = new Serial("\\\\.\\COM8");    // adjust as needed

	//if (SP->IsConnected())
	//	printf("We're connected");

	//char incomingData[256] = "";			// don't forget to pre-allocate memory
	////printf("%s\n",incomingData);
	//int dataLength = 255;
	//int readResult = 0;

	//while (SP->IsConnected())
	//{

	//	readResult = SP->ReadData(incomingData, dataLength);
	//	// printf("Bytes read: (0 means no data available) %i\n",readResult);
	//	incomingData[readResult] = 0;

	//	printf("%s", incomingData);

	//	SP->WriteData("1", 1);
	//	printf("send 1\n");
	//	
	//	Sleep(500);
	//}


	/* For image processing test */
	//for (int i = 1; i < 11; i++) {
	//	char buf[256];
	//	sprintf_s(buf, "carImage/%d.jpg", i);
	//	imageProcessing(buf);
	//	printCarNumber("carImage/temp.jpg");

	//	Mat image = imread("carImage/temp.jpg");
	//	sprintf_s(buf, "carImage/%d-1.jpg", i);
	//	imwrite(buf,image);
	//}

	//imageProcessing("carImage/capturedImage.jpg");
	//printCarNumber("carImage/temp.jpg");

	return 0;
}