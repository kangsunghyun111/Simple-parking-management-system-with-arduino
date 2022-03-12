#define PARKINGLOT 8	// Num of parking spaces
#define LED 8			// Num of LED
#define SECTOR 2		// Num of Sector
#define DISTANCE 5.0	// Distance to detect

bool parkingSpace[PARKINGLOT] = { 0 };	// Empty is 0, full is 1
int echo[PARKINGLOT] = {};
int trig[PARKINGLOT] = {};
int greenLED[LED] = {};
int redLED[LED] = {};
int guideGreenLED[SECTOR] = {};
int guideRedLED[SECTOR] = {};

int carDetected = 0;		// For interaction with car detection code

void sensorInit();			// Initialize ultra sonic sensors
void LEDInit();				// Initialize LED
void sensorCheck();			// Sensor detection and print distance
void parkingLotPrint();		// Print parking spaces
void parkingLotLED();		// Manage parking space LED
void sectorLED();			// Manage sector LED
void turnOffPower();		// If parking lot is empty, turn off all LEDs
bool isEmpty();				// If parking lot is empty, return 1

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);
	sensorInit();
	LEDInit();
	carDetected = 1;	
}

// the loop function runs over and over again until power down or reset
void loop() {
	if (carDetected == 1) {
		sensorCheck();		// Renew parking spaces with sensors
		parkingLotLED();	// Decide led state
		sectorLED();		// Decide guideLED state
		parkingLotPrint();	// Print parking spaces
		delay(500);
	}
}

void sensorInit() {
	int i;
	for (i = 0; i < PARKINGLOT; i++) {
		// Initialize sensors
		pinMode(trig[i], OUTPUT);
		pinMode(echo[i], INPUT);
	}
}

void LEDInit() {
	int i;
	for (i = 0; i < LED; i++) {
		// Initialize leds
		pinMode(greenLED[i], OUTPUT);
		digitalWrite(greenLED[i], LOW);
		pinMode(redLED[i], OUTPUT);
		digitalWrite(redLED[i], LOW);
	}
	for (i = 0; i < SECTOR; i++) {
		// Initialize guideLED
		pinMode(guideGreenLED[i], OUTPUT);
		digitalWrite(guideGreenLED[i], LOW);
		pinMode(guideRedLED[i], OUTPUT);
		digitalWrite(guideRedLED[i], LOW);
	}
}

void sensorCheck() {
	int i;
	float duration, distance;
	for (i = 0; i < PARKINGLOT; i++) {
		// Trigger signal
		digitalWrite(trig[i], LOW);		// Initialization
		delay(2);
		digitalWrite(trig[i], HIGH);	// Signal
		delay(10);
		digitalWrite(trig[i], LOW);

		// Input echo signal
		duration = pulseIn(echo[i], HIGH);

		// Calculate distance
		distance = ((float)(340 * duration) / 10000) / 2;

		// Print distance
		Serial.print("Sensor[");
		Serial.print(i);
		Serial.print("] : ");
		Serial.print(distance);
		Serial.println("cm");

		// If detected distance is shorter than DISTANCE, suppose the parking space is used 
		if (parkingSpace[i] == 0) {
			if (distance <= DISTANCE) {
				parkingSpace[i] = 1;
			}
		}
		else if (parkingSpace[i] == 1) {
			if (distance > DISTANCE) {
				parkingSpace[i] = 0;
			}
		}
	}
}

void parkingLotPrint() {
	int i;
	for (i = 0; i < PARKINGLOT; i++) {
		// Print state of parking space
		Serial.print("parkingSpace[");
		Serial.print(i);
		Serial.print("] : ");
		if (parkingSpace[i] == 0)
			Serial.println("empty");
		else
			Serial.println("full");
	}
}

void parkingLotLED() {
	int i;
	for (i = 0; i < LED; i++) {
		// If parking space is empty, turn on LED
		if (parkingSpace[i] == 0) {
			digitalWrite(greenLED[i], HIGH);
			digitalWrite(redLED[i], LOW);
		}
		else {
			digitalWrite(greenLED[i], LOW);
			digitalWrite(redLED[i], HIGH);
		}
	}
}

void sectorLED() {
	int i, j;
	int sum = 0;
	for (i = 0; i < SECTOR; i++) {
		// Check parking spaces in the SECTOR
		for (j = 0; j < (LED / SECTOR); j++) {
			sum += parkingSpace[j];
		}

		// If there is even one parking space, turn on thr guideLED
		if (sum == (LED / SECTOR)) {
			pinMode(guideGreenLED[i], LOW);
			pinMode(guideRedLED[i], HIGH);
		}
		else {
			pinMode(guideGreenLED[i], HIGH);
			pinMode(guideRedLED[i], HIGH);
		}
	}
}

void turnOffPower() {
	int i;
	for (i = 0; i < LED; i++) {
		digitalWrite(greenLED[i], LOW);
		digitalWrite(redLED[i], LOW);
	}
	for (i = 0; i < SECTOR; i++) {
		digitalWrite(guideGreenLED[i], LOW);
		digitalWrite(guideRedLED[i], LOW);
	}
}

bool isEmpty() {
	int i;
	int sum = 0;
	for (i = 0; i < PARKINGLOT; i++) {
		sum += parkingSpace[i];
	}

	if (sum == 0) {	// Parking lot is empty
		return 1;
	}
	else {			// Parking lot is not empty
		return 0;
	}
}