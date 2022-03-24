#include "CarDetection.h"
#define parkingSpace 10	// Number of parking spaces
#define retry 3	// If can not recognize car lisence plate, retry 3 times

carInfo* carList[parkingSpace];
String cars_cascade_name = "haarcascade_russian_plate_number.xml";
String window_name = "Capture - car detection";
string carNumber;
CascadeClassifier cars_cascade;
char detectStatus = 0;
bool carDetected = false;
char temp = '0';
int carCount = 0;
int attempt = 0;
int index = -1;
double parkingTime = 0;

// For opencv print
string tempEnterCar;
time_t tempEnterTime;
string tempOutCar;
time_t tempOutTime;
time_t tempParkingTime;

// For Serial communications with arduino
int arduinoSerial = 0;


void readVideoStream() {
	VideoCapture capture;
	Mat frame;
	char buffer;
	bool check = false;

	// Load cascade
	if (!cars_cascade.load(cars_cascade_name)) {
		printf("--(!)Error loading cars cascade\n"); 
		return; 
	}

	// Read the video stream
	capture.open("sampleVideo.mp4");
	
	if (!capture.isOpened()) {
		printf("--(!)Error opening video capture\n"); 
		return; 
	}

	// Print video resolution
	int videoWidth = capture.get(CAP_PROP_FRAME_WIDTH);
	int videoHeight = capture.get(CAP_PROP_FRAME_HEIGHT);
	cout << videoWidth << " " << videoHeight << '\n';

	// Init carList
	for (int i = 0; i < parkingSpace; i++)
		carList[i] = new carInfo();

	// Connect arduino
	Serial* SP = new Serial("\\\\.\\COM8");    // adjust as needed
	if (SP->IsConnected())
		printf("\n\nWe're connected with arduino\n");

	char incomingData[256] = "";			// don't forget to pre-allocate memory
	//printf("%s\n",incomingData);
	int dataLength = 255;
	int readResult = 0;

	while (SP->IsConnected())
	{
		// Video processing
		while (capture.read(frame))
		{
			if (frame.empty())
			{
				printf(" --(!) No captured frame -- Break!");
				break;
			}

			// Apply the classifier to the frame
			carDetectAndDisplay(frame,SP);

			int c = waitKey(10);
			if ((char)c == 27)
				break;	// escape
		}	

		delete SP;
		break;
	}


}

void carDetectAndDisplay(Mat frame, Serial* SP)
{
	
	carDetected = false;
	vector<Rect> cars;
	Mat frame_gray;
	Mat display;
	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	// Detect car license plates
	cars_cascade.detectMultiScale(frame_gray, cars, 1.1, 3, 0 | CASCADE_SCALE_IMAGE, Size(100, 50),Size(300,100));


	if (detectStatus == 0) {
		if (cars.size() > 0) {
			detectStatus = 1;	// Change status

			// Capture an image with a car license plate and process the image 
			imwrite("carImage/temp.jpg", frame);
			imageProcessing("carImage/temp.jpg");

			// Get car number. If the license plate is not recognized, try three more times
			carNumber = printCarNumber("carImage/temp.jpg");
			if (carNumber == "0") {
				cout << "Can't recognize carPlate\n";
				if (attempt == retry) {	// If tried enough, skip it
					detectStatus = 1;
					attempt = 0;
				}
				else {
					cout << "Retry. . ." << '\n';
					detectStatus = 0;
					attempt++;
				}
			}
			else if (carCount == parkingSpace) {	// If parking lot is full
				cout << "Parking lot is full\n";
			}
			else {
				index = checkCarPlate(carNumber);
				if (index == -1) {
					int i;
					for (i = 0; i < parkingSpace; i++) {
						if (carList[i]->getCarPlate() == "") {	// Find empty carList space
							carList[i]->setCarPlate(carNumber);
							carList[i]->setEnterTime(time(NULL));
							carDetected = true;
							SP->WriteData("1", 1);
							printf("send 1\n");
							break;
						}
					}
					cout << carList[i]->getCarPlate() << " enterd at " << carList[i]->getEnterTime() << '\n';

					// For opencv window
					tempEnterCar = carList[i]->getCarPlate();
					tempEnterTime = carList[i]->getEnterTime();
					carCount++;
					carDetected = true;
					SP->WriteData("1", 1);
					printf("send 1\n");
				}
				else {
					carList[index]->setOutTime(time(NULL));
					parkingTime = carList[index]->getParkingTime();
					if (parkingTime > 5) {
						cout << carList[index]->getCarPlate() << " out at " << carList[index]->getOutTime() << '\n';
						cout<<carList[index]->getCarPlate()<<" parking time is "<< carList[index]->getParkingTime() << " sec\n";

						// For opencv window
						tempOutCar = carList[index]->getCarPlate();
						tempOutTime = carList[index]->getOutTime();
						tempParkingTime = carList[index]->getParkingTime();

						// Initialization
						carList[index]->setCarPlate("");
						carList[index]->setEnterTime(-1);
						carList[index]->setOutTime(-1);
						carCount--;
						carDetected = false;
					}
				}
			}
		}
		else {
			// Do nothing
		}
	}
	else if (detectStatus == 1) {
		if (cars.size() > 0) {
			// Do nothing
		}
		else {
			SP->WriteData("0", 1);
			printf("send 0\n");
			detectStatus = 0;
		}
	}

	// Rectangle car lisence plate
	for (size_t i = 0; i < cars.size(); i++)
	{
		Point center(cars[i].x + cars[i].width / 2, cars[i].y + cars[i].height / 2);
		rectangle(frame, Point(cars[i].x, cars[i].y), Point(cars[i].x + cars[i].width, cars[i].y + cars[i].height), Scalar(0, 255, 0), 4, 8, 0);
		Mat faceROI = frame_gray(cars[i]);
	}

	// Display parking lot information
	display = imread("background.jpg");
	string myText = "Parking Lot Information";
	Point myPoint = Point(10, 40);
	int myFontFace = 2;
	double myFontScale = 1.2;
	putText(display, myText, myPoint, myFontFace, myFontScale, Scalar(0, 0, 0));

	// Print parking time
	string carNumberText;
	string informationText;
	string temp;
	Point carPoint;
	time_t carParkingTime;
	for (int i = 0; i < parkingSpace; i++) {
		if (carList[i]->getCarPlate() != "") {
			informationText = "";
			carNumberText = carList[i]->getCarPlate();
			carParkingTime = time(NULL) - carList[i]->getEnterTime();
			temp = to_string(carParkingTime);

			carPoint = Point(10, 80 + (i * 40));
			informationText.append(carNumberText);
			informationText.append(" : ");
			informationText.append(temp);
			informationText.append(" sec");
			putText(display, informationText, carPoint, myFontFace, myFontScale, Scalar(0, 0, 0));
		}
	}

	// Print enter and out car
	if ((time(NULL) - tempEnterTime) < 5) {
		myText = tempEnterCar;
		myPoint = Point(10, 520);
		myText.append(" is enterd");
		putText(display, myText, myPoint, myFontFace, myFontScale, Scalar(0, 0, 255));
	}
	if ((time(NULL) - tempOutTime) < 5) {
		myText = tempOutCar;
		myPoint = Point(10, 560);
		temp = to_string(tempParkingTime);

		myText.append(" out, parking time : ");
		myText.append(temp);
		myText.append(" sec");
		putText(display, myText, myPoint, myFontFace, myFontScale, Scalar(0, 0, 255));
	}

	// Show what you got
	imshow("Capture - Car Detection", frame);	
	imshow("Parking Lot Information", display);
}

int checkCarPlate(string carNumber) {
	string temp;
	for (int i = 0; i < parkingSpace; i++) {
		temp =  carList[i]->getCarPlate();
		if (temp == carNumber) {
			return i;
		}
	}
	return -1;
}