# 1 "/Users/gcg004/Documents/ledstrip-control/eased-onoff/eased-onoff.ino"
# 1 "/Users/gcg004/Documents/ledstrip-control/eased-onoff/eased-onoff.ino"
# 2 "/Users/gcg004/Documents/ledstrip-control/eased-onoff/eased-onoff.ino" 2
# 12 "/Users/gcg004/Documents/ledstrip-control/eased-onoff/eased-onoff.ino"
QuarticEase ease;

// for attiny85

const byte ledStrip1Pin = 1; // Connection for the ledstrip (via ULN2803) PB1
const byte ledStrip2Pin = 0; // Connection for the ledstrip (via ULN2803) PB0
const byte switchPin = 4; // pin number to connect the switch(es) to PB4
const byte pirPin = 3; // Optional PIR PB3

double easedPosition, t = 0;

int switchState; // the current reading from the input pin
int lastSwitchState = 0x0; // the previous reading from the input pin
// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0; // the last time the output pin was toggled
unsigned long debounceDelay = 50; // the debounce time; increase if the output flickers

void setup()
{
  ease.setDuration(5); //has to be made dynamic later
  ease.setTotalChangeInPosition(255);

  /*
  pinMode(ledStrip1Pin, OUTPUT);
  pinMode(ledStrip2Pin,OUTPUT);
  pinMode(switchPin, INPUT);
  pinMode(pirPin,INPUT);
  */

  // set input and output mode via DDRB register (instead of pinMode)
  (*(volatile uint8_t *)((0x17) + 0x20)) |= (0 << 4) | (0 << 3) | (1 << 1) | (1 << 0);

  // Run once after reboot of the arduino
  smoothOn();
  delay(2000);
  smoothOff();
}

void loop()
{
  t = 0;
  // read the state of the door switches
  int reading = 0x0;

  if ((((*(volatile uint8_t *)((0x16) + 0x20))) & ((0x01 << (4)))) /* Get the value of a bit, like bitGet(PORTB, BIT(5));*/)
  {
    reading = 0x1;
  }
  else
  {
    reading = 0x0;
  }

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
      if ((switchState == 0x1) and (easedPosition == 0))
      {
        // Serial.println("On");
        smoothOn();
      }

      if ((switchState == 0x0) and (easedPosition > 0))
      {
        smoothOff();
        //  Serial.println("Off");
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
    analogWrite(ledStrip1Pin, (unsigned char)easedPosition);
    analogWrite(ledStrip2Pin, (unsigned char)easedPosition);
    t += 0.005;
    delay(5);
  }
  easedPosition = 255;

  // PORTB |= (1<<ledStrip2Pin) | (1<<ledStrip1Pin);
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

  //  PORTB &= ~(1<<ledStrip2Pin)  ~(1<<ledStrip1Pin);
  easedPosition = 0;
}
