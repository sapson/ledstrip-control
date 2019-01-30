#include <EasingLibrary.h>
#define bitGet(p,m) ((p) & (m)) // Get the value of a bit, like bitGet(PORTB, BIT(5));
#define bitSet(p,m) ((p) |= (m)) // Set the value of a bit (set it to 1), like bitSet(PORTB, BIT(2));
#define bitClear(p,m) ((p) &= ~(m)) // Clear a bit (set it to 0), like bitClear(PORTB, BIT(2));
#define bitFlip(p,m) ((p) ^= (m))
#define bitWrite(c,p,m) (c ? bit_set(p,m) : bit_clear(p,m))
#define BIT(x) (0x01 << (x))
#define LONGBIT(x) ((unsigned long)0x00000001 << (x))



QuarticEase ease;

// for attiny85

const byte ledStrip1Pin = 1; // Connection for the ledstrip (via ULN2803) PB1
const byte ledStrip2Pin = 0; // Connection for the ledstrip (via ULN2803) PB0
const byte switchPin = 4;    // pin number to connect the switch(es) to PB4
const byte pirPin = 2;       // Optional PIR PB2
const byte potPin = 3;       // Optional Potentiometer to PB3

double easedPosition, t = 0;

int switchState;           // the current reading from the input pin
int lastSwitchState = LOW; // the previous reading from the input pin
// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0; // the last time the output pin was toggled
unsigned long debounceDelay = 50;   // the debounce time; increase if the output flickers

int maxValue = 255;       // the max value for the led brighthness

void setup()
{
  ease.setDuration(2); //has to be made dynamic later
  ease.setTotalChangeInPosition(255);

  /*
  pinMode(ledStrip1Pin, OUTPUT);
  pinMode(ledStrip2Pin,OUTPUT);
  pinMode(switchPin, INPUT);
  pinMode(pirPin,INPUT);
  */

  // set input and output mode via DDRB register (instead of pinMode)
  DDRB |= (0 << DDB4) | (0 << DDB3) | (0 << DDB2) | (1 << DDB1) | (1 << DDB0);

  // Run once after reboot of the arduino
  smoothOn();
  delay(2000);
  smoothOff();
}

void loop()
{
  t = 0;
  // read the state of the door switches
  int reading = LOW;

  // read the state of the Potentiometer
  int potReading = (int) analogRead(potPin) / 4;
  // if brighthness has changed, and led is already on at maxvalue, adapt the brighthness
  if (potReading != maxValue) {
    maxValue = potReading;
    if (easedPosition > 0) {
       analogWrite(ledStrip1Pin, (unsigned char)maxValue);
       analogWrite(ledStrip2Pin, (unsigned char)maxValue);
    }
  }
  // reset reading to LOW
  reading = LOW;

  // if SWITCH is OPEN then set HIGH
  if (bitGet(PINB,BIT(4)))
  {
    reading = HIGH;
  } 
  // if PIR detected movement then set HIGH
  if (bitGet(PINB,BIT(2)))
  {
    reading = HIGH;
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
      maxValue = potReading;

      // check if the pushbutton is pressed. If it is, the switchState is HIGH:
      if ((switchState == HIGH) and (easedPosition == 0))
      {
        // Serial.println("On");
        smoothOn();
      }

      if ((switchState == LOW) and (easedPosition > 0))
      {
        smoothOff();
        //  Serial.println("Off");
      }
    }
  }

  lastSwitchState = reading;
}

void rapidOn() {
    // turn LED strip on without any smoothing
    analogWrite(ledStrip1Pin, (unsigned char)maxValue);
    analogWrite(ledStrip2Pin, (unsigned char)maxValue);
    delay(5);
    easedPosition = maxValue;
}

void smoothOn()
{
  // turn LED strip on:

  while (easedPosition < maxValue)
  {
    easedPosition = ease.easeInOut(t);
    analogWrite(ledStrip1Pin, (unsigned char)easedPosition);
    analogWrite(ledStrip2Pin, (unsigned char)easedPosition);
    t += 0.005;
    delay(5);
  } //
  easedPosition = maxValue;

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
