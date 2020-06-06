//int seg[7] = {A0, 3, A1, A2, 6, 7, 8};
int seg[7] = {A5, A4, A3, A2, A1, A0, 2};
/*
seg[0] = seg a = port 2 van Arduino Uno
seg[1] = seg b = port 3
seg[2] = seg c = port 4
seg[3] = seg d = port 5
seg[4] = seg e = port 6
seg[5] = seg f = port 7
seg[6] = seg g = port 8
*/
 
//int CC[4] = {9, 10, 11, 12};
int CC[4] = {9, 10, 11, 12};
/*
CC[0] = port 9 van Arduino Uno = LSD (rightmost) CC=Common Cathode
CC[1] = port 10
CC[2] = port 11
CC[3] = port 12 = MSD (leftmost) Most Significant Digit
*/

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
          0b0000000, //leeg
} ; 

void toonDuizendtal(int duizendtal)
{
  for (int i=0;i<=6;i++)
  { 
    digitalWrite(seg[i],bitRead(onDisplay[duizendtal],i));
  }

  digitalWrite(CC[0],HIGH);
  digitalWrite(CC[1],HIGH);
  digitalWrite(CC[2],HIGH);
  digitalWrite(CC[3],LOW);
  delay(1);
  digitalWrite(CC[3],HIGH);
}

void toonHonderdtal(int honderdtal)
{
  for (int i=0;i<=6;i++)
  { 
    digitalWrite(seg[i],bitRead(onDisplay[honderdtal],i));
  }

  digitalWrite(CC[0],HIGH);
  digitalWrite(CC[1],HIGH);
  digitalWrite(CC[2],LOW);
  digitalWrite(CC[3],HIGH);
  delay(1);
  digitalWrite(CC[2],HIGH);
}

void toonTiental(int tiental)
{
  for (int i=0;i<=6;i++)
  { 
    digitalWrite(seg[i],bitRead(onDisplay[tiental],i));
  }

  digitalWrite(CC[0],HIGH);
  digitalWrite(CC[1],LOW);
  digitalWrite(CC[2],HIGH);
  digitalWrite(CC[3],HIGH);
  delay(1);
  digitalWrite(CC[1],HIGH);
}

void toonEenheid(int eenheid)
{
  for (int i=0;i<=6;i++)
  { 
    digitalWrite(seg[i],bitRead(onDisplay[eenheid],i));
  }

  digitalWrite(CC[0],LOW);
  digitalWrite(CC[1],HIGH);
  digitalWrite(CC[2],HIGH);
  digitalWrite(CC[3],HIGH);
  delay(1);
  digitalWrite(CC[0],HIGH);
}

void toonUren(int uren)
{
  if(uren < 10)
  {
    toonDuizendtal(16);
    //delay(2.5);
    toonHonderdtal(uren);
    //delay(2.5);
  }
  else
  {
    toonDuizendtal(uren / 10);
    delay(2.5);
    toonHonderdtal(uren % 10);
    delay(2.5);
  }
}

void toonMinuten(int minuten)
{
  toonTiental(minuten / 10);
  //delay(2.5);
  toonEenheid(minuten % 10);
  //delay(2.5);
}

void toonCijfer(int cijfer) 
{

  int duizendtal = cijfer / 1000;
  int honderdtal = (cijfer % 1000) / 100;
  int tiental = (cijfer % 100) / 10;
  int eenheid = (cijfer % 10);
  toonDuizendtal(duizendtal);
  delay(2.5);
  toonHonderdtal(honderdtal);
  delay(2.5);
  toonTiental(tiental);
  delay(2.5);
  toonEenheid(eenheid);
  delay(2.5);
  //for (int i=0;i<=6;i++)
  // { digitalWrite(seg[i],bitRead(onDisplay[cijfer],i));
  // }
}

void setup() 
{
 for (int i=0;i<=6;i++){pinMode(seg[i], OUTPUT);};
 for (int i=0;i<=3;i++){pinMode(CC[i], OUTPUT);};
}  

void loop() {
   // Simple counter
   /*
   for(int i = 0; i < 10000; i++)
   {
    toonCijfer(i);
    toonCijfer(i);
    toonCijfer(i);
    toonCijfer(i);
    toonCijfer(i);
   }
   */
   // Clock
   for (int uren = 0; uren < 24; uren++)
   {
    for(int minuten = 0; minuten < 60; minuten++)
    {
      for (int j = 0; j < 10; j++)
      {
        toonUren(uren);
        toonMinuten(minuten);
        toonUren(uren);
        toonMinuten(minuten);
        toonUren(uren);
      }
    }
   }
 }
