#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
  #include "SoftwareSerial.h"
  SoftwareSerial port(12,13);
#else // Arduino 0022 - use modified NewSoftSerial
  #include "WProgram.h"
  #include "NewSoftSerial.h"
  NewSoftSerial port(12,13);
#endif

#include "EasyVR.h"
EasyVR easyvr(port);

//////////////
int ledPins[] = { 9, 10, 11 };

const boolean ON = LOW;
const boolean OFF = HIGH;

const boolean RED[] = {ON, OFF, OFF};
const boolean GREEN[] = {OFF, ON, OFF};
const boolean BLUE[] = {OFF, OFF, ON};
const boolean YELLOW[] = {ON, ON, OFF};
const boolean CYAN[] = {OFF, ON, ON};
const boolean MAGENTA[] = {ON, OFF, ON};
const boolean WHITE[] = {ON, ON, ON};
const boolean BLACK[] = {OFF, OFF, OFF};
const boolean* COLORS[] = {RED, YELLOW, GREEN, CYAN, BLUE, MAGENTA, WHITE, BLACK};

boolean state[] = {OFF, OFF, OFF};
//////////////

//Groups and Commands
enum Groups
{
  GROUP_0  = 0,
  GROUP_1  = 1,
};

enum Group0 
{
  G0_POWER_ON = 0,
};

enum Group1 
{
  G1_RED = 0,
  G1_BLUE = 1,
  G1_GREEN = 2,
  G1_YELLOW = 3,
  G1_WHITE = 4,
  G1_CYAN = 5,
  G1_MAGENTA = 6,
  G1_PAUSE = 7,
  G1_FUCK = 8,
  G1_POWER_OFF = 9,
};


EasyVRBridge bridge;

int8_t group, idx;

void setup()
{
  // bridge mode?
  if (bridge.check())
  {
    cli();
    bridge.loop(0, 1, 12, 13);
  }
  // run normally
  Serial.begin(9600);
  port.begin(9600);

  if (!easyvr.detect())
  {
    Serial.println("EasyVR not detected!");
    for (;;);
  }

  easyvr.setPinOutput(EasyVR::IO1, LOW);
  Serial.println("EasyVR detected!");
  easyvr.setTimeout(5);
  easyvr.setLanguage(0);

  group = EasyVR::TRIGGER; //<-- start group (customize)
  
  //////
  for(int i = 0; i < 3; i++)
    pinMode(ledPins[i], OUTPUT);

  stateToLED();
  //////
}

void action();

void loop()
{
  
  
  easyvr.setPinOutput(EasyVR::IO1, HIGH); // LED on (listening)

  Serial.print("Say a command in Group ");
  Serial.println(group);
  easyvr.recognizeCommand(group);

  do
  {
    setColorToState(BLACK);
    // can do some processing while waiting for a spoken command
  }
  while (!easyvr.hasFinished());
  
  easyvr.setPinOutput(EasyVR::IO1, LOW); // LED off

  idx = easyvr.getWord();
  if (idx >= 0)
  {
    // built-in trigger (ROBOT)
    // group = GROUP_X; <-- jump to another group X
    return;
  }
  idx = easyvr.getCommand();
  if (idx >= 0)
  {
    // print debug message
    uint8_t train = 0;
    char name[32];
    Serial.print("Command: ");
    Serial.print(idx);
    if (easyvr.dumpCommand(group, idx, name, train))
    {
      Serial.print(" = ");
      Serial.println(name);
    }
    else
      Serial.println();
    easyvr.playSound(0, EasyVR::VOL_FULL);
    // perform some action
    action();
  }
  else // errors or timeout
  {
    if (easyvr.isTimeout())
      Serial.println("Timed out, try again...");
    int16_t err = easyvr.getError();
    if (err >= 0)
    {
      Serial.print("Error ");
      Serial.println(err, HEX);
    }
  }
  
  
}

void action()
{
    switch (group)
    {
    case GROUP_0:
      switch (idx)
      {
      case G0_POWER_ON:
        group = GROUP_1;
        setColor(ledPins, GREEN);
        delay(100);
        setColor(ledPins, BLACK);
        delay(100);
        setColor(ledPins, GREEN);
        delay(100);
        // write your action code here
        // group = GROUP_X; <-- or jump to another group X for composite commands
        break;
      }
      break;
    case GROUP_1:
      switch (idx)
      {
      case G1_RED:
        setColorToState(RED);
        // write your action code here
        // group = GROUP_X; <-- or jump to another group X for composite commands
        break;
      case G1_BLUE:
        setColorToState(BLUE);
        // write your action code here
        // group = GROUP_X; <-- or jump to another group X for composite commands
        break;
      case G1_GREEN:
        setColorToState(GREEN);
        // write your action code here
        // group = GROUP_X; <-- or jump to another group X for composite commands
        break;
      case G1_YELLOW:
        setColorToState(YELLOW);
        // write your action code here
        // group = GROUP_X; <-- or jump to another group X for composite commands
        break;
      case G1_WHITE:
        setColorToState(WHITE);
        // write your action code here
        // group = GROUP_X; <-- or jump to another group X for composite commands
        break;
      case G1_CYAN:
        setColorToState(CYAN);
        // write your action code here
        // group = GROUP_X; <-- or jump to another group X for composite commands
        break;
      case G1_MAGENTA:
        setColorToState(MAGENTA);
        // write your action code here
        // group = GROUP_X; <-- or jump to another group X for composite commands
        break;
      case G1_PAUSE:
        setColorToState(BLACK);
        // write your action code here
        // group = GROUP_X; <-- or jump to another group X for composite commands
        break;
      case G1_FUCK:
        setColor(ledPins, RED);
        delay(500);
        setColor(ledPins, GREEN);
        delay(500);
        setColor(ledPins, BLUE);
        delay(500);
        // write your action code here
        // group = GROUP_X; <-- or jump to another group X for composite commands
        break;
      case G1_POWER_OFF:
        setColor(ledPins, RED);
        delay(100);
        setColor(ledPins, BLACK);
        delay(100);
        setColor(ledPins, RED);
        delay(100);
        group = GROUP_0;
        // write your action code here
        // group = GROUP_X; <-- or jump to another group X for composite commands
        break;
      }
      break;
    }
    stateToLED();

}

void setColorToState(const boolean* color)
{
  for(int i = 0; i < 3; i++)
  state[i] = color[i];
}

void stateToLED() {
  for(int i = 0; i < 3; i++)
    digitalWrite(ledPins[i], state[i]);
}

void setColor(int* led, const boolean* color)
{
  for(int i = 0; i < 3; i++)
    digitalWrite(led[i], color[i]);     
}
