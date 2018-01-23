#include <ZumoMotors.h>
#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>
#include <Pushbutton.h>

#define LED_PIN       13 // user LED pin

//Var to interact with the zumo hardware
ZumoMotors motors;
ZumoReflectanceSensorArray sensors(QTR_NO_EMITTER_PIN);
Pushbutton button(ZUMO_BUTTON);
unsigned int sensor_values[6];

//Var to check if the zumo is going forwards down a corridor (task 2)
boolean forwardCheck;
int corridorNumber = 0;

//Var to check if the room is on the left or right hand side of the corridor 
//(left = true, right = false)
//and to record the room number
boolean roomCheck;
int roomNum = 0;
void setup()
{
sensors.init();
  
button.waitForButton();
// initial calibration of the sensors
delay(1000);
int i;
for (i = 0; i < 80; i++)
{
  if ((i > 10 && i <= 30) || (i > 50 && i <= 70))
    motors.setSpeeds(-200, 200);
  else
    motors.setSpeeds(200, -200);
  sensors.calibrate();
  delay(20);
}
motors.setSpeeds(0, 0);
//end sensor calibration

pinMode(LED_PIN, OUTPUT);

//Start the serial so arduino can communicate to processing
Serial.begin(9600);

}

void loop()
{
//integer varialbes to control the speed over the left and right motor of the zumo  
  int left_speed = 0;
  int right_speed = 0;

  delay(50);
  
//If the serial registers a value from processing then continue with the statement    
  if (Serial.available() > 0)
  {
//read the value given from processing and assign it to the var motor
    char motor = toUpperCase((char) Serial.read());
//random characters are sent through from processing to carry out a specific instruction
//in this case, if the letter 'Q' is sent through the boolean varialbe forwardCheck will be changed to true
    if (motor == 'Q')
    {
      corridorNumber = corridorNumber + 1;
      String tempStore1 = String(corridorNumber);
      String tempStore2 = "Now Traversing Corridor #" + tempStore1 + ".";
      Serial.println(tempStore2);
      forwardCheck = true;
    }

//for the initial control of the zumo (task 1) this switch statement is in place
//for which ever character is called the speed of both motors are set 
//and a message is sent to processing 
    switch (motor) {
      case 'A':
        left_speed = -200;
        right_speed = 200;
        Serial.println("Turning Left 90 degrees");
        roomCheck = true;
        break;
      case 'W':
        left_speed = 200;
        right_speed = 200;
        Serial.println("Moving Forward");
        break;
      case 'S':
        left_speed = -200;
        right_speed = -200;
        Serial.println("Moving Back");
        break;
      case 'D':
        left_speed = 200;
        right_speed = -200;
        Serial.println("Turning Right 90 degrees");
        roomCheck = false;
        break;
      case 'P':
//the letter P is used to stop the zumo in by setting the speed to 0,0
//it also sets the bool variable of forwardCheck to false to stop
        forwardCheck = false;
        Serial.println("Breaking...");
        left_speed = 0;
        right_speed = 0;
        break;
      case 'R':
      String roomName = "";
      if(roomCheck){roomName = "Left";}
      if(!roomCheck){roomName = "Right";}
      roomNum = roomNum + 1;
      String tempstore = "Now entering the" + roomName +" # " +roomNum; 
        Serial.println(tempstore); 
    }
//once the speed has been set then send the instructions to the zumo
//with a delay 
    motors.setSpeeds(left_speed, right_speed);
    delay(300);
    motors.setSpeeds(0, 0);
  }

  if (forwardCheck == true)
  {
    sensors.read(sensor_values);
    if((sensor_values[0] >= sensors.calibratedMaximumOn[0]) && (sensor_values[5] >= sensors.calibratedMaximumOn[5]))
    {
      //if both left and right sensors detect a line then stop and wait for user interaction
      forwardCheck = false;
      Serial.println("User assistance requested");
      motors.setSpeeds(0, 0);
    }                                         
    else if (sensor_values[0] >= sensors.calibratedMaximumOn[0])  // 900
    {
      // if leftmost sensor detects line, reverse and turn to the right
      Serial.println("Left Sensor Triggered -- Altering Course");
      motors.setSpeeds(-200, -200);
      delay(200);
      motors.setSpeeds(200, -200);
      delay(300);
      motors.setSpeeds(200, 200);
    }
    else if (sensor_values[5] >= sensors.calibratedMaximumOn[5])
    {
      
      // if rightmost sensor detects line, reverse and turn to the left
      Serial.println("Right Sensor Triggered -- Altering Course");
      motors.setSpeeds(-200, -200);
      delay(200);
      motors.setSpeeds(-200, 200);
      delay(300);
      motors.setSpeeds(200, 200);
    }
    else
    {
      // otherwise, go straight
      motors.setSpeeds(200, 200);
    }
  }

}
