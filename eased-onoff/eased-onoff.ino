
#define ledStrip1Pin 6 // Connection for the ledstrip (via ULN2803)
#define ledStrip2Pin 5 // Connection for the ledstrip (via ULN2803)
#define switchPin 3    // pin number to connect the switch(es) to
#define pirPin 7       // Optional PIR
#define speedPin A3    // Optional potentiometer to control the fade on time

double easedPosition, t = 0;

int switchState, pirState; // the current reading from the input pin
int lastSwitchState = LOW; // the previous reading from the input pin
int lastPirState = LOW;
// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0; // the last time the output pin was toggled
unsigned long debounceDelay = 50;   // the debounce time; increase if the output flickers

void smoothOn()
{
  // turn LED strip on:
  while (easedPosition < 255)
  {
    // easedPosition = ease.easeInOut(t);
    analogWrite(ledStrip1Pin, (unsigned char)easedPosition);
    analogWrite(ledStrip2Pin, (unsigned char)easedPosition);
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
    analogWrite(ledStrip1Pin, (unsigned char)easedPosition);
    analogWrite(ledStrip2Pin, (unsigned char)easedPosition);
    delay(5);
  }
  easedPosition = 0;
}

void setup()
{
  pinMode(ledStrip1Pin, OUTPUT);
  pinMode(ledStrip2Pin, OUTPUT);
  pinMode(switchPin, INPUT);
  pinMode(pirPin, INPUT);

  /* default 5 second duration*/
  // ease.setDuration(5);
  // ease.setTotalChangeInPosition(255);

  pinMode(speedPin, INPUT);

  /* overwrite if we have speed control */
  //   ease.setDuration(analogRead(speedPin) * (5.0 / 1024.0));

  // Run once after reboot/reset of the arduino to provide visual feedback
  smoothOn();
  delay(2000);
  smoothOff();
}

void loop()
{
  t = 0;

  int reading = digitalRead(pirPin);

  if (reading != lastPirState)
  {
    {
      pirState = reading;

      // check if the pushbutton is pressed. If it is, the switchState is HIGH:
      if ((pirState == HIGH) and (easedPosition == 0))
      {
        smoothOn();
      }

      if ((pirState == LOW) and (easedPosition > 0))
      {
        smoothOff();
      }
    }
  }
  lastPirState = reading;
  /*
  * only check for the state of the switch when the PIR is LOW 
  * to avoid that the switch could turn of the light when it is turned on by the PIR
  */
  if (lastPirState == LOW)
  {
    reading = digitalRead(switchPin);

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
          smoothOn();
        }

        if ((switchState == LOW) and (easedPosition > 0))
        {
          smoothOff();
        }
      }
    }
    lastSwitchState = reading;
  }
}
