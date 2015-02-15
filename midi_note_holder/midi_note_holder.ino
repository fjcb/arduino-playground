#include <Bounce.h>

//see board/board.fzz for further information about the pins
int ledPin1 = 5;
int ledPin2 = 3;

int potiPin1 = A0;
int potiPin2 = A1;

int buttonPin1 = 2;
int buttonPin2 = 7;

Bounce bouncer1 = Bounce(buttonPin1, 5);
Bounce bouncer2 = Bounce(buttonPin2, 5);

//low b and e, additional octaves
int bs[] = {47, 59, 71, 83};
int es[] = {52, 64, 76, 88};

//# of polyphone octaves
int octaves = 1;

//enum for notes
enum notes { B, E };

//initial values
int velocity = 0x45;
boolean holding = false;
notes note = B;

void setup()
{
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  
  //midi baudrate
  Serial.begin(31250);
}

void loop()
{
  bouncer1.update();
  bouncer2.update();
  
  //velocity poti
  velocity = (analogRead(potiPin1) >> 3) - 1; //division by 8:   [0..127]
  octaves = min(4, max(1, (analogRead(potiPin2) >> 8))); //division by 256: [0..3] (array index for notes array)
  
  //check button 1 for note decision
  if(bouncer1.fallingEdge())
  {
    stopNotes(0x90);
    
    if(note == B)
    {
      note = E;
      digitalWrite(ledPin1, HIGH);
    }
    else
    {
      note = B;
      digitalWrite(ledPin1, LOW);
    }
  }
  
  //check button 2 for sending note on/off
  if(bouncer2.fallingEdge())
  {
    if(holding)
    {
      stopNotes(0x90);
      holding = false;
      digitalWrite(ledPin2, LOW);
    }
    else
    {
      if(note == B)
      {
        playNotes(0x90, bs);
      }
      else
      {
        playNotes(0x90, es);
      }
      
      holding = true;
      digitalWrite(ledPin2, HIGH);
    }
  }
  
  //delay(50);
}

void sendNote(int cmd, int note, int vel)
{
  Serial.write(cmd);
  Serial.write(note);
  Serial.write(vel);
}

void stopNotes(int cmd)
{
  holding = false;
  digitalWrite(ledPin2, LOW);
  
  for(int i = 0; i < 4; i++)
  {
    sendNote(cmd, bs[i], 0x00);
    sendNote(cmd, es[i], 0x00);
  }
}

void playNotes(int cmd, int noteArray[])
{
  for(int i = 0; i < octaves; i++)
  {
    sendNote(cmd, noteArray[i], velocity);
  }
}


