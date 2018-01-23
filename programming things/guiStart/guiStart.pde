import processing.serial.*;
import g4p_controls.*;

Serial ardPort;  // Create object from Serial class
String val = " ";     // Data received from the serial port
boolean lineCheck = false;

int bgcol = 15;

public void setup() {
  String portName = "COM8"; //change the 0 to a 1 or 2 etc. to match your port
  ardPort = new Serial(this, portName, 9600);

  size(600, 400);
  createGUI();
}

public void draw() {
  background(200, 200, 200);
  if ( ardPort.available() > 0) 
  {  // If data is available,
  
  val = val + ardPort.readStringUntil('\n');         // read it and store it in val
  val= val.replaceAll("null", "");

  textarea1.setText(val); 
  } 
}