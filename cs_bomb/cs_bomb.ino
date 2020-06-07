// GEBRUIK INTERRUPTS VOOR SECONDEN TIMER
// GEBRUIK INTERRUPTS VOOR PIEPER

#include "cs_bomb_lib.h"

int seg[7] = {A5, A4, A3, A2, A1, A0, 4};
/*
seg[0] = seg a = port 2
seg[1] = seg b = port 3
seg[2] = seg c = port 4
seg[3] = seg d = port 5
seg[4] = seg e = port 6
seg[5] = seg f = port 7
seg[6] = seg g = port 8
*/

int CC[4] = {9, 10, 11, 12};
/*
CC[0] = port 9 van Arduino Uno = LSD (rightmost) CC=Common Cathode
CC[1] = port 10
CC[2] = port 11
CC[3] = port 12 = MSD (leftmost) Most Significant Digit
*/

const int arm_button = 3;
const int disarm_button = 2;
const int arming_led = 5;
const int buzzer = 6;
const int disarming_led = 7;

byte onDisplay[17] = 
{
// bits     6543210
// segments gfedcba         
          0b0111111, //0
          0b0000110, //1
          0b1011011, //2
          0b1001111, //3
          0b1100110, //4
          0b1101101, //5
          0b1111101, //6
          0b0000111, //7
          0b1111111, //8
          0b1101111, //9
          0b1110111, //A
          0b1111100, //b
          0b0111001, //C
          0b1011110, //d
          0b1111001, //E
          0b1110001, //F
          0b0000000, //empty
} ; 

void displayNumber(int number, int display_n)
{
  for (int i=0;i<=6;i++)
  { 
    digitalWrite(seg[i],bitRead(onDisplay[number],i));
  }
  
  for(int i = 0; i < 4; i++)
  {
    if(i == display_n)
    {
      digitalWrite(CC[i],LOW);
    }
    else
    {
      digitalWrite(CC[i],HIGH);
    }
  }
  delay(1);
  digitalWrite(CC[display_n],HIGH);
  

}

void showHours(int hours)
{
  if(hours < 10)
  {
    displayNumber(16, 3);
    displayNumber(hours, 2);
  }
  else
  {
    displayNumber(hours / 10, 3);
    displayNumber(hours % 10, 2);
  }
}

void showMinutesRight(int minutes)
{
  displayNumber(minutes / 10, 1);
  displayNumber(minutes % 10, 0);
}

void showMinutesLeft(int minutes)
{
  displayNumber(minutes / 10, 3);
  displayNumber(minutes % 10, 2);
}

void showSeconds(int seconds)
{
  displayNumber(seconds / 10, 1);
  displayNumber(seconds % 10, 0);
}

void showHoursMinutes(int hours, int minutes)
{
  //Duration: 4 ms
  showHours(hours);
  showMinutesRight(minutes);
}

void showMinutesSeconds(int minutes, int seconds)
{
  //Duration: 4 ms
  showMinutesLeft(minutes);
  showSeconds(seconds);
}

void timeToMinutesSeconds(int seconds)
{
  int m = seconds / 60;
  int s = seconds % 60;
  showMinutesSeconds(m, s);
}

// Variables will change:
int buttonState_arm;             // the current reading from the input pin
int buttonState_disarm;          // the current reading from the input pin
int lastButtonState_arm = LOW;   // the previous reading from the input pin
int lastButtonState_disarm = LOW;// the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime_arm = 0;     // the last time the output pin was toggled
unsigned long lastDebounceTime_disarm = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

// Timing of how long the button has been held
unsigned long lastPressTime_arm = millis();
unsigned long lastReleaseTime_arm = millis();
unsigned long holdTime_arm = millis();

unsigned long lastPressTime_disarm = millis();
unsigned long lastReleaseTime_disarm = millis();
unsigned long holdTime_disarm = millis();


states bomb_state = INIT;
//int bomb_timer = BOMB_TIMER;
// The bomb timer variable decrements at a rate of 125Hz -> 125 decrements equal 1 second
long bomb_timer = (long) BOMB_TIMER * (long) TIMER_CLK;
long counter = 0;

bool beep = false;
unsigned long lastBeepStart = 0;


void debounce(int buttonPin, int * lastButtonState, int * buttonState, unsigned long * lastDebounceTime, unsigned long * lastPressTime, unsigned long * holdTime, unsigned long * lastReleaseTime)
{
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != *lastButtonState) {
    // reset the debouncing timer
    *lastDebounceTime = millis();
  }

  if ((millis() - *lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != *buttonState) 
    {
      *buttonState = reading;

      if(reading == HIGH)
      {
        //Serial.println("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
        *lastPressTime = millis();
      }
      else
      {
        //Serial.println("BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB");
        *lastReleaseTime = millis();
      }
    }
    if(*buttonState == HIGH)
    {
      //Serial.println("CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC");
      *holdTime = millis();
    }
    
  }
  *lastButtonState = reading;
}

states check_arming()
{
  debounce(arm_button, &lastButtonState_arm, &buttonState_arm, &lastDebounceTime_arm, &lastPressTime_arm, &holdTime_arm, &lastReleaseTime_arm);

  //Serial.print(digitalRead(arm_button));
  //Serial.print(" ");
  //Serial.print(holdTime_arm);
  //Serial.print(" ");
  //Serial.print(lastPressTime_arm);
  //Serial.print(" ");
  //Serial.print(holdTime_arm > lastPressTime_arm);
  //Serial.print(" ");
  //Serial.print(buttonState_arm == HIGH);
  //Serial.print(" ");
  //Serial.println(holdTime_arm - lastPressTime_arm);
  
  //Serial.print(holdTime_arm);
  //Serial.print(" ");
  //Serial.print(lastPressTime_arm);
  //Serial.print(" ");
  //Serial.println(buttonState_arm);
  if((holdTime_arm > lastPressTime_arm)&& ((holdTime_arm - lastPressTime_arm) > ARM_TIME))
  {
    return ARMED;
  }
  else if((holdTime_arm > lastPressTime_arm) && (buttonState_arm == HIGH))
  {
    return ARMING;
  }
  else
  {
    return INIT;
  }
}

states check_disarming()
{
  debounce(disarm_button, &lastButtonState_disarm, &buttonState_disarm, &lastDebounceTime_disarm, &lastPressTime_disarm, &holdTime_disarm, &lastReleaseTime_disarm);

  if((holdTime_disarm > lastPressTime_disarm)&& ((holdTime_disarm - lastPressTime_disarm) > DISARM_TIME))
  {
    //Serial.println("DEFUSED");
    return DEFUSED;
  }
  else if((holdTime_disarm > lastPressTime_disarm) && (buttonState_disarm == HIGH))
  {
    //Serial.println("DEFUSING");
    return DEFUSING;
  }
  else
  {
    //Serial.println("ARMED");
    return ARMED;
  }
}

void startBeep()
{
  lastBeepStart = millis();
  beep = true;
  tone(buzzer, 1000);
}

void stopBeep()
{
  beep = false;
  noTone(buzzer);
}

ISR(TIMER1_COMPA_vect)
{
  long beep_duration;
  if ((bomb_state == ARMED) || (bomb_state == DEFUSING))
  {
    // If applicable, decrement the timer counter
    if(bomb_timer > 0) bomb_timer--;

    // Calculate the remaining time before explosion
    long rem_time = bomb_timer / (long) TIMER_CLK;

    // Apply beeping depeding on this remaining time
    // Stage 1: start -> 1:00
    if(rem_time > STAGE2_REM_TIME)
    {
      if((counter % STAGE1_MOD) == 0)
      {
        startBeep();
      }
      beep_duration = STAGE1_MOD / 2;
    }
    // Stage 2: 1:00 -> 0:30
    else if(rem_time > STAGE3_REM_TIME)
    {
      if((counter % STAGE2_MOD) == 0)
      {
        startBeep();
      }
      beep_duration = STAGE2_MOD / 2;
    }
    // Stage 3: 0:30 -> 0:10
    else if(rem_time > STAGE4_REM_TIME)
    {
      if((counter % STAGE3_MOD) == 0)
      {
        startBeep();
      }
      beep_duration = STAGE3_MOD / 2;
    }
    // Stage 4: 0:10 -> 0:05
    else if(rem_time > STAGE5_REM_TIME)
    {
      if((counter % STAGE4_MOD) == 0)
      {
        startBeep();
      }
      beep_duration = STAGE4_MOD / 2;
    }
    // Stage 5: 0:05 -> 0:00
    else
    {
      if((counter % STAGE5_MOD) == 0)
      {
        startBeep();
      }
      beep_duration = STAGE5_MOD / 2;
    }
  }
  else if((bomb_state == ARMING) || (bomb_state == DEFUSING))
  {
    startBeep();
    //if((counter % ARMING_MOD) == 0)
    //{
    //  startBeep();
    //}
    //beep_duration = ARMING_MOD / 2;
    beep_duration = 1000000;
  }

  //Disable any running beep
  if(millis() >= (lastBeepStart + beep_duration))
  {
    stopBeep();
    
  }
  counter++;
}

void explode()
{
  bool on = false;
  for(int i = 0; i < 20; i++)
  {
    if(on)
    {
      digitalWrite(disarming_led, LOW);
      digitalWrite(arming_led, LOW);
      tone(buzzer, 500);
    }
    else
    {
      digitalWrite(disarming_led, HIGH);
      digitalWrite(arming_led, HIGH);
      tone(buzzer, 2000);
      
    }
    on != on;
    delay(100);
  }
  noTone(buzzer);
}

void set_interrupt_timers()
{
  cli();//stop interrupts

  //set timer1 interrupt at 125Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  //OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  OCR1A = 124;// = (16*10^6) / (125*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  /*
  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B
  TCNT2  = 0;//initialize counter value to 0
  // set compare match register for 8khz increments
  OCR2A = 249;// = (16*10^6) / (8000*8) - 1 (must be <256)
  // turn on CTC mode
  TCCR2A |= (1 << WGM21);
  // Set CS21 bit for 8 prescaler
  TCCR2B |= (1 << CS21);   
  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);
  */
  sei();//allow interrupts
}

void setup() 
{
 for (int i=0;i<=6;i++){pinMode(seg[i], OUTPUT);};
 for (int i=0;i<=3;i++){pinMode(CC[i], OUTPUT);};
 pinMode(arm_button, INPUT);
 pinMode(disarm_button, INPUT);
 pinMode(arming_led, OUTPUT);
 Serial.begin(9600);
 set_interrupt_timers();
 Serial.println(STAGE1_MOD);
}

void loop() 
{
  if((bomb_state == INIT) || (bomb_state == ARMING))
  {
    //long rem_time = bomb_timer;
    //Serial.print(rem_time);
    //Serial.print(" ");
    //Serial.print(BOMB_TIMER);
    //Serial.print(" ");
    //Serial.print(TIMER_CLK);
    //Serial.print(" ");
    //Serial.println(BOMB_TIMER * TIMER_CLK);
    //Serial.println("test");
    states checked_state = check_arming();
    
    if(checked_state == ARMED)
    {
      bomb_state = ARMED;
      counter = 0;
      stopBeep();
      digitalWrite(arming_led, LOW);
    }
    else if(checked_state == ARMING)
    {
      bomb_state = ARMING;
      digitalWrite(arming_led, HIGH);
      //tone(buzzer, 1000);
      //delay(250);
      //noTone(buzzer);
      //delay(250);
    }
    else
    {
      bomb_state = INIT;
      digitalWrite(arming_led, LOW);
    }
  }
  else if((bomb_state == ARMED) || (bomb_state == DEFUSING))
  {
    if(bomb_timer == 0)
    {
      digitalWrite(disarming_led, LOW);
      bomb_state = EXPLODING;
      Serial.println("EXPLODING");
      //cli();//stop interrupts
    }
    else
    {
      states checked_state = check_disarming();
      if(checked_state == DEFUSED)
      {
        bomb_state = DEFUSED;
      }
      else if(checked_state == DEFUSING)
      {
        digitalWrite(disarming_led, HIGH);
        bomb_state = DEFUSING;
      }
      else
      {
        digitalWrite(disarming_led, LOW);
        bomb_state = ARMED;
      }
    }
    timeToMinutesSeconds(bomb_timer / (long) TIMER_CLK);
  }
  else if(bomb_state == EXPLODING)
  {
    bomb_state = EXPLODED;
    Serial.println("BOEM");
    explode();
  }
  else if(bomb_state == DEFUSED)
  {
    digitalWrite(disarming_led, HIGH);
    digitalWrite(arming_led, LOW);
    timeToMinutesSeconds(bomb_timer / (long) TIMER_CLK);
  }
  else
  {
    digitalWrite(disarming_led, LOW);
    digitalWrite(arming_led, HIGH);
    timeToMinutesSeconds(bomb_timer / (long) TIMER_CLK);
  }
 }
