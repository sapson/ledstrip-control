#include <EasingLibrary.h>

QuarticEase ease;

const int ledStripPin = 9;   // Connection for the ledstrip (via ULN2803) 
const int switchPin = 2;     // pin number to connect the switches to 
/*
    PIN 2 --- Switch --- 10 K Ohm resistor --- GROUND
                     --- 5V between switch and 10 K Ohm 
    Pull down configuration
    Switch -> Normal Open configuration             
*/

double easedPosition, t = 0;

int switchState;             // the current reading from the input pin
int lastSwitchState = LOW;   // the previous reading from the input pin
// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0; // the last time the output pin was toggled
unsigned long debounceDelay = 50;   // the debounce time; increase if the output flickers

void setup()
{
  
  Serial.begin(9600);
  Serial.println("Debugging started");
  delay(2000); // Give reader a chance to see the output.
  
  ease.setDuration(5);
  ease.setTotalChangeInPosition(255);

  pinMode(ledStripPin, OUTPUT);
  pinMode(switchPin, INPUT);

  // Run once after reboot of the arduino
  smoothOn();
  delay(2000);
  smoothOff();
}

void loop()
{
  t = 0;
  // read the state of the door switches
  int reading = digitalRead(switchPin);

  if (reading != lastSwitchState)
  {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != switchState)
    {
      switchState = reading;

      // check if the pushbutton is pressed. If it is, the switchState is HIGH:
      if ((switchState == HIGH) and (easedPosition == 0))
      {
        Serial.println("On");
        smoothOn();
      }

      if ((switchState == LOW) and (easedPosition > 0))
      {
        smoothOff();
        Serial.println("Off");
      }
    }
  }

  lastSwitchState = reading;
}

void smoothOn()
{
  // turn LED strip on:
  while (easedPosition < 255)
  {
    easedPosition = ease.easeInOut(t);
    analogWrite(ledStripPin, (unsigned char)easedPosition);
    t += 0.02;
    delay(5);
  }
  easedPosition = 255;
}

void smoothOff()
{
  // turn LED strip on:
  while (easedPosition > 0)
  {
    easedPosition -= 1;
    analogWrite(ledStripPin, (unsigned char)easedPosition);
    delay(5);
  }
  easedPosition = 0;
}
