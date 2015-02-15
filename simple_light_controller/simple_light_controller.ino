/*
*  Simple Light Controller
*   - DMX RGB LED PAR Spots
*   - Analog Strobe
* 
*  Strobe is connected on pin 9 (see board/board.fzz)
*/

#include <DmxSimple.h>
#include <Bounce.h>

const int led_1 = 3;
const int led_2 = 5;
const int strobe = 9;

const int button_1 = 2;
const int button_2 = 4;
const int button_3 = 7;
const int poti_1 = A0; 

unsigned long last_hit = 1000;
unsigned int tap_tempo = 1000;

int tap_time = 1000;
unsigned long lastStrobeMilis = 0;
unsigned long lastTapMilis = 0;

boolean black_out = true;
boolean led_state = false;

Bounce bouncer_1 = Bounce(button_1, 5);
Bounce bouncer_2 = Bounce(button_2, 5);
Bounce bouncer_3 = Bounce(button_3, 5);
int button_state_1 = LOW;
int button_state_2 = LOW;
int button_state_3 = HIGH;

unsigned int strobe_time = 1000;
boolean strobe_active = false;

void setup() 
{                
  pinMode(led_1, OUTPUT); 
  pinMode(led_2, OUTPUT);
  pinMode(strobe, OUTPUT); 
  
  pinMode(button_1, INPUT); 
  pinMode(button_2, INPUT);
  
  DmxSimple.usePin(6);
  DmxSimple.maxChannel(10);
  
  randomSeed(42);
}

boolean cycleCheck(unsigned long *lastMillis, unsigned int cycle) 
{
  unsigned long currentMillis = millis();
  if(currentMillis - *lastMillis >= cycle)
  {
    *lastMillis = currentMillis;
    return true;
  }
  else
    return false;
}

void blackOutLed()
{
  if(black_out)
  {
    if(!led_state)
    {
      digitalWrite(led_2, HIGH); 
      led_state = true;
    }
  }
  else
  {
    if(led_state)
    {
      digitalWrite(led_2, LOW); 
      led_state = false;
    }
  }
}

void processBouncer()
{
  if(bouncer_1.update()) 
  {
    button_state_1 = bouncer_1.read();
  }
  
  if(bouncer_2.update())
  {
    button_state_2 = bouncer_2.read();
  }  
  
  if(bouncer_3.update())
  {
    button_state_3 = bouncer_3.read(); 
  }
}

void processButton1()
{
  //calculate black out
  if(button_state_1 == HIGH)
  {
    black_out = !black_out; 
    button_state_1 = LOW;
  }
}

void processButton2()
{
  if(button_state_2 == HIGH)
  {
    tap_time = max(100, millis() - last_hit);
    last_hit = millis();
    button_state_2 = LOW;
  }
}

void processButton3()
{
  if(button_state_3 == LOW)
  {
    strobe_active = true;
  }
  else
  {
    strobe_active = false; 
  }
}

void dmxNextColor()
{  
  if(!black_out && !strobe_active)
  {   
    DmxSimple.write(2, random(2) ? 0 : 255);
    DmxSimple.write(3, random(2) ? 0 : 255);
    DmxSimple.write(4, random(2) ? 0 : 255);
    
    DmxSimple.write(7, random(2) ? 0 : 255);
    DmxSimple.write(8, random(2) ? 0 : 255);
    DmxSimple.write(9, random(2) ? 0 : 255); 
  }
}

void dmxBlackOut()
{
  if(black_out || strobe_active)
  {
    DmxSimple.write(2, 0);
    DmxSimple.write(3, 0);
    DmxSimple.write(4, 0);
    
    DmxSimple.write(7, 0);
    DmxSimple.write(8, 0);
    DmxSimple.write(9, 0);
  } 
}

void loop() 
{
  strobe_time = max(100, analogRead(poti_1));  
  
  processBouncer();
  processButton1();
  processButton2();
  processButton3();

  //strobe
  if(cycleCheck(&lastStrobeMilis, strobe_time) && strobe_active && !black_out)
  {
    digitalWrite(strobe, HIGH); 
    delay(1);
    digitalWrite(strobe, LOW);
  }
  
  //tap tempo led
  if(cycleCheck(&lastTapMilis, tap_time))
  {
    dmxNextColor();
    
    digitalWrite(led_1, HIGH); 
    delay(1);
    digitalWrite(led_1, LOW); 
  }

  dmxBlackOut();
  
  blackOutLed();  
}
