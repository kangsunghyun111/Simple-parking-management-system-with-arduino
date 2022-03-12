#include "CarDetection.h"
#include "ImageProcessing.h"
#include "ParkingManagement.h"
#define parkingSpace 10
#define retry 3

String cars_cascade_name = "haarcascade_russian_plate_number.xml";
String window_name = "Capture - car detection";
string carNumber;
CascadeClassifier cars_cascade;
char detectStatus = 0;
char temp = '0';
int carCount = 0;
int attempt = 0;
int index = -1;
double parkingTime = 0;

carInfo* carList[parkingSpace];

void readVideoStream() {
	VideoCapture capture;
	Mat frame;
	char buffer;

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

	// Video processing
	while (capture.read(frame))
	{
		if (frame.empty())
		{
			printf(" --(!) No captured frame -- Break!");
			break;
		}

		// Apply the classifier to the frame
		carDetectAndDisplay(frame);
		int c = waitKey(10);
		if ((char)c == 27)
			break;	// escape
	}
}

void carDetectAndDisplay(Mat frame)
{
	vector<Rect> cars;
	Mat frame_gray;
	Mat display;
	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	// Detect cars
	cars_cascade.detectMultiScale(frame_gray, cars, 1.1, 3, 0 | CASCADE_SCALE_IMAGE, Size(100, 50),Size(300,100));

	if (detectStatus == 0) {
		if (cars.size() > 0) {
			detectStatus = 1;

			imwrite("carImage/temp.jpg", frame);
			imageProcessing("carImage/temp.jpg");
			carNumber = printCarNumber("carImage/temp.jpg");
			if (carNumber == "0") {
				cout << "Can't recognize carPlate\n";
				if (attempt == retry) {
					detectStatus = 1;
					attempt = 0;
				}
				else {
					cout << "Retry. . ." << '\n';
					detectStatus = 0;
					attempt++;
				}
			}
			else if (carCount > parkingSpace) {
				cout << "Parking lot is full\n";
			}
			else {
				index = checkCarPlate(carNumber);
				if (index == -1) {
					//carList[carCount] = new carInfo(carNumber, time(NULL));
					int i;
					for (i = 0; i < parkingSpace; i++) {
						if (carList[i]->getCarPlate() == "") {
							carList[i]->setCarPlate(carNumber);
							carList[i]->setEnterTime(time(NULL));
							break;
						}
					}
					cout << carList[i]->getCarPlate() << " enterd at " << carList[i]->getEnterTime() << '\n';
					carCount++;
				}
				else {
					carList[index]->setOutTime(time(NULL));
					parkingTime = carList[index]->getParkingTime();
					if (parkingTime > 5) {
						cout << carList[index]->getCarPlate() << " out at " << carList[index]->getOutTime() << '\n';
						cout<<carList[index]->getCarPlate()<<" parking time is "<< carList[index]->getParkingTime() << " sec\n";
						//delete carList[index];
						carList[index]->setCarPlate("");
						carList[index]->setEnterTime(-1);
						carList[index]->setOutTime(-1);
						carCount--;
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
			detectStatus = 0;
		}
	}

	// Rectangle car
	for (size_t i = 0; i < cars.size(); i++)
	{
		Point center(cars[i].x + cars[i].width / 2, cars[i].y + cars[i].height / 2);
		rectangle(frame, Point(cars[i].x, cars[i].y), Point(cars[i].x + cars[i].width, cars[i].y + cars[i].height), Scalar(0, 255, 0), 4, 8, 0);
		Mat faceROI = frame_gray(cars[i]);
	}

	// Display parking lot information
	display = imread("background.jpg");
	string myText = "testing. . .";
	Point myPoint = Point(10, 40);
	int myFontFace = 2;
	double myFontScale = 1.2;
	putText(display, myText, myPoint, myFontFace, myFontScale, Scalar(0, 0, 0));

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
	imshow("Parking Lot Information", display);

	// Show what you got
	imshow("Capture - Car Detection", frame);
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