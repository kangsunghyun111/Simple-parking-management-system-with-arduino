#define PARKINGLOT 8  // Num of parking spaces
#define LED 8     // Num of LED
#define SECTOR 2    // Num of Sector
#define DISTANCE 5.0  // Distance to detect

bool parkingSpace[PARKINGLOT] = { 0 };  // Empty is 0, full is 1
int echo[PARKINGLOT] = { 30,32,35,37,39,41,43,45 };
int trig[PARKINGLOT] = { 31,33,34,36,38,40,42,44 };
int greenLatch = 9;
int greenClock = 10;
int greenData = 8;
int redLatch = 3;
int redClock = 4;
int redData = 2;
int greenLED[LED] = {};
int redLED[LED] = {};
int guideGreenLED[SECTOR] = { 50,51 };
int guideRedLED[SECTOR] = { 52,53 };
String input;
int guideCount;

int carDetected = 0;    // For interaction with car detection code

void sensorInit();      // Initialize ultra sonic sensors
void LEDInit();       // Initialize LED
void sensorCheck();     // Sensor detection and print distance
void parkingLotPrint();   // Print parking spaces
void parkingLotLED();   // Manage parking space LED
void guideLED();     // Manage sector LED
void turnOffPower();    // If parking lot is empty, turn off all LEDs
bool isEmpty();       // If parking lot is empty, return 1

// the setup function runs once when you press reset or power the board
void setup() {
    Serial.begin(9600);
    sensorInit();
    LEDInit();
}

// the loop function runs over and over again until power down or reset
void loop() {
    sensorCheck();    // Renew parking spaces with sensors
    parkingLotLED();  // Decide led state
    guideLED();    // Decide guideLED state
    parkingLotPrint();  // Print parking spaces
    delay(1000);
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
    int leds = 255;
    // Initialize shift registers
    pinMode(greenLatch, OUTPUT);
    pinMode(greenClock, OUTPUT);
    pinMode(greenData, OUTPUT);
    pinMode(redLatch, OUTPUT);
    pinMode(redClock, OUTPUT);
    pinMode(redData, OUTPUT);

    // Initialize green leds
    digitalWrite(greenLatch, LOW);
    shiftOut(greenData, greenClock, MSBFIRST, 0);
    digitalWrite(greenLatch, HIGH);

    // Initialize red leds
    digitalWrite(redLatch, LOW);
    shiftOut(redData, redClock, MSBFIRST, 0);
    digitalWrite(redLatch, HIGH);
        
    // Initialize guideLED
    for (i = 0; i < SECTOR; i++) {
        pinMode(guideGreenLED[i], OUTPUT);
        digitalWrite(guideGreenLED[i], HIGH);
        pinMode(guideRedLED[i], OUTPUT);
        digitalWrite(guideRedLED[i], HIGH);
    }
}

void sensorCheck() {
    int i;
    float duration, distance;
    for (i = 0; i < PARKINGLOT; i++) {
        // Trigger signal
        digitalWrite(trig[i], LOW);   // Initialization
        delay(2);
        digitalWrite(trig[i], HIGH);  // Signal
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
    int leds = 0;
    for (i = 0; i < LED; i++) {
        if (parkingSpace[i] == 0) {
            Serial.print(i);

            leds += 1 << i;
        }
    }
    Serial.println();
    Serial.println(leds);

    // If parking space is empty, turn on green LED
    digitalWrite(greenLatch, LOW);
    shiftOut(greenData, greenClock, MSBFIRST, leds);
    digitalWrite(greenLatch, HIGH);

    // If parking space is empty, turn on red LED
    digitalWrite(redLatch, LOW);
    shiftOut(redData, redClock, MSBFIRST, 255 - leds);
    digitalWrite(redLatch, HIGH);
}

void guideLED() {
    int i, j;
    int sum = 0;
    if (Serial.available()) {
        Serial.println("serial available");
        input = Serial.read();
        Serial.print(input);
        Serial.println(" is input");
        // If Visual Studio detected car, it send "1" and "1" is 49 in ASCII code
        if (input.equals("49")) {
            // If car detected, set guideCount for guideleds working
            guideCount = 1;
            Serial.println("input is 1, guideCount = 4");
        }
    }
    if (guideCount == 0) {
        // If car doesn't detect, turn off guideleds 
        for (i = 0; i < SECTOR; i++) {
            digitalWrite(guideGreenLED[i], LOW);
            digitalWrite(guideRedLED[i], LOW);
        }
    }
    if (guideCount > 0) {
        guideCount--;
        for (i = 0; i < SECTOR; i++) {
            sum = 0;
            // Check parking spaces in the SECTOR
            for (j = 0; j < (LED / SECTOR); j++) {
                sum += parkingSpace[(4 * i) + j];
            }

            // If there is even one parking space, turn on thr guideLED
            if (sum == (LED / SECTOR)) {
                digitalWrite(guideGreenLED[i], LOW);
                digitalWrite(guideRedLED[i], HIGH);
            }
            else {
                digitalWrite(guideGreenLED[i], HIGH);
                digitalWrite(guideRedLED[i], LOW);
            }
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

    if (sum == 0) { // Parking lot is empty
        return 1;
    }
    else {      // Parking lot is not empty
        return 0;
    }
}