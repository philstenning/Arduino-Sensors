
#include "Arduino.h"
#include <NewPing.h>
#include <Wire.h>

////////////////////////////////////////////////
// Each time a request is made from the master device
// a value from the sensorValue array is returned.
// The sensorValue array is looped through and
// depending on the i2cMode, a different ranges of
// values are returned.
// When a value is received from the master device it changes
// the i2cMode
///////////////////////////////////////////////
unsigned long previousMillis = 0; // will store last time for loop
const long interval = 100;        // time  of loop

// the i2c addess used by both the master and slave device
#define SLAVE_ADDRESS 0x04

// sensor board pinout
// sensor : trigger : Echo
// 1      : 2       : 5
// 2      : 7       : 6
// 3      : 8       : 3
// 4      : 9       : 4

//Sensor 1
#define TRIGGER_PIN_S1 2
#define ECHO_PIN_S1 5 // you  can use a single pin for trrigger/echo if the bord is a 5v one.
//Sensor 2
#define TRIGGER_PIN_S2 7
#define ECHO_PIN_S2 6
//Sensor 3
#define TRIGGER_PIN_S3 8
#define ECHO_PIN_S3 3
//Sensor 4
#define TRIGGER_PIN_S4 9
#define ECHO_PIN_S4 4

#define MAX_DISTANCE 200 // max distance in cm. same for all

//initialise the sensors
NewPing sonar1(TRIGGER_PIN_S1, ECHO_PIN_S1, MAX_DISTANCE);
NewPing sonar2(TRIGGER_PIN_S2, ECHO_PIN_S2, MAX_DISTANCE);
NewPing sonar3(TRIGGER_PIN_S3, ECHO_PIN_S3, MAX_DISTANCE);
NewPing sonar4(TRIGGER_PIN_S4, ECHO_PIN_S4, MAX_DISTANCE);

// if you change the sonars array change
// this number as well..
// remember it is a zero based array
const int NUMBER_OF_SENSORS = 4;

// create an array of sonar sensors
NewPing sonars[NUMBER_OF_SENSORS] = {
    sonar1,
    sonar2,
    sonar3,
    sonar4};

//not used now
//int returnValueForI2c = 0;

// i2cMode is set by the master device
int i2cMode = 1; // ALL = 1, ULTRASONIC = 2

//initialise the sensors values
//int sensorValues[4]={3,7,13,27};
int sensorValues[4] = {0, 0, 0, 0};

// callback for received data
void receiveData(int byteCount)
{

  // Serial.print("byteCount: ");
  // Serial.println(byteCount);
  while (Wire.available())
  {

    int num = Wire.read();
    Serial.print("data received: ");
    Serial.println(num);

    switch (num)
    {
    case 2:
      i2cMode = 2; //ULTRASONIC ;
      break;

    default:
      i2cMode = 1; //ALL ;
      break;
    }
  }
}

int sensorNum = 0;
void sendAllData()
{

  Wire.write(sensorValues[sensorNum]);
  Serial.print(" sensorNum:");
  Serial.print(sensorNum);
  if (sensorNum < (NUMBER_OF_SENSORS - 1))
  {
    sensorNum++;
  }
  else
  {
    sensorNum = 0;
    Serial.print("\n");
  }
 
}

// callback for sending data
void sendData()
{
  switch (i2cMode)
  {
  case 2:
    //do something when var equals 1
    break;

  default:
    sendAllData();
    break;
  }
}

void setup()
{
  Serial.begin(115200); // start serial for output
  // initialize i2c as slave
  Wire.begin(SLAVE_ADDRESS);

  // define callbacks for i2c communication
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);

  Serial.println("Ready!");
}

void printValue(int sensorId, int distance)
{
  Serial.print("PingSensor");
  Serial.print(++sensorId);
  Serial.print(": ");
  Serial.print(distance);
  Serial.println("cm");
}
void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;

    for (int i = 0; i < NUMBER_OF_SENSORS; i++)
    {
      int tempval = sonars[i].ping_cm();

      //only change value if it has changed.
      if (tempval != sensorValues[i])
      {
        //print values to serial
        printValue(i, tempval);
        //set values
        sensorValues[i] = tempval;
      }
    }
  }
}
