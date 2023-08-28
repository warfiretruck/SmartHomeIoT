#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define BUTTONANALOG A7
#define STX 3
#define SRX 2

#define BUZZER 13
#define DOOSERVO 12
#define FIRESENSOR A0
#define PIRSENSOR A1
#define GASSENSOR A2


#define KeyCol1 6
#define KeyCol2 5
#define KeyCol3 4

#define KeyRowA 10
#define KeyRowB 9
#define KeyRowC 8
#define KeyRowD 7

#define NOKEY 12
#define KEY0 0
#define KEY1 1
#define KEY2 2
#define KEY3 3
#define KEY4 4
#define KEY5 5
#define KEY6 6
#define KEY7 7
#define KEY8 8
#define KEY9 9
#define KEYSTAR 10
#define KEYHASH 11

#define STAMAIN 0
#define STACHANGEPASS 1
#define STACHECKPASS 2
#define STAUNLOCKBYCODE 3
#define STASETUPARLARM 4

#define INACTIVE 1
#define ACTIVE 0

#define ON 1
#define OFF 0

#define ESPCOMMAND 0xB1

#define ARLARMFIRE 0xA4
#define ARLARMSERCURITY 0xA5
#define ARLARMGAS 0xA6

#define BUTTONLIGHT1 0xA8
#define BUTTONFAN1 0xA9
#define BUTTONLIGHT2 0xAA
#define BUTTONFAN2 0xAB
#define BUTTONLIGHT3 0xAC
#define BUTTONFAN3 0xAD

int ButtonAnalogVals[] = {1022,409,244,131,93,64,0};
byte ButtonCommand[]={NOKEY,BUTTONFAN3,BUTTONLIGHT3,BUTTONFAN2,BUTTONLIGHT2,BUTTONFAN1,BUTTONLIGHT1};
const int SmallestGap = 30; // smallest value differrent between analog button
byte CurrentState=STAMAIN;
byte NextState;

Servo DoorServo;


LiquidCrystal_I2C LCDDisplay(0x27,16,2);
SoftwareSerial DataSerial(SRX,STX); //TX is 9 RX is 8 pin


byte CurrentPass[6];
byte PassWord[6];
byte CurrentPassIndex;


byte ArlarmStatus=INACTIVE;
byte FIREArlarmActive=INACTIVE;
byte PIRArlarmActive=INACTIVE;
byte GASArlarmActive=INACTIVE;

void SendDataToESP(byte DataType,byte DataValue)
{
  DataSerial.write(DataType);
  DataSerial.write(DataValue);
}



void SavePasswordToEEPROM()
{
  EEPROM.write(0,PassWord[0]);
  EEPROM.write(1,PassWord[1]);
  EEPROM.write(2,PassWord[2]);
  EEPROM.write(3,PassWord[3]);
  EEPROM.write(4,PassWord[4]);
  EEPROM.write(5,PassWord[5]);
  
}


void GetPasswordFromEEPROM()
{
     
      if(EEPROM.read(0)==0xFF)// blank eeprom save default pass
      {
        PassWord[0]=0;
        PassWord[1]=2;
        PassWord[2]=4;
        PassWord[3]=6;
        PassWord[4]=8;
        PassWord[5]=1;
        SavePasswordToEEPROM();
        return;
      }
      else
      {
        
          PassWord[0]=EEPROM.read(0);
          PassWord[1]=EEPROM.read(1);
          PassWord[2]=EEPROM.read(2);
          PassWord[3]=EEPROM.read(3);
          PassWord[4]=EEPROM.read(4);
          PassWord[5]=EEPROM.read(5);
      }
  
}

bool VerifyPass()
{
  byte i;
  
  for(i=0;i<6;i++)
  {
    if(CurrentPass[i]!=PassWord[i]) return false;
  }

  return true;
}

void DisPlayCheckPass()
{
  LCDDisplay.clear();
  LCDDisplay.setCursor(3,0);
  LCDDisplay.print("NHAP MAT MA:"); 
  LCDDisplay.setCursor(3,1);

  CurrentPassIndex=0;
  CurrentPass[0]=0;
  CurrentPass[1]=0;
  CurrentPass[2]=0;
  CurrentPass[3]=0;
  CurrentPass[4]=0;
  CurrentPass[5]=0;
}

  
byte ScanKey(void)
{
     digitalWrite(KeyRowA,HIGH); 
     digitalWrite(KeyRowB,HIGH); 
     digitalWrite(KeyRowC,HIGH); 
     digitalWrite(KeyRowD,HIGH); 
     
     digitalWrite(KeyRowA,LOW);
     if(!digitalRead(KeyCol1)) 
     { 
      while(!digitalRead(KeyCol1));
      delay(10);
      return KEY1;
     }
     if(!digitalRead(KeyCol2)) 
     { 
      while(!digitalRead(KeyCol2));
      delay(10);
      return KEY2;
     }
      if(!digitalRead(KeyCol3)) 
     { 
      while(!digitalRead(KeyCol3));
      delay(10);
      return KEY3;
     }
    
     digitalWrite(KeyRowA,HIGH);
     digitalWrite(KeyRowB,LOW);
     
    if(!digitalRead(KeyCol1)) 
     { 
      while(!digitalRead(KeyCol1));
      delay(10);
      return KEY4;
     }
     if(!digitalRead(KeyCol2)) 
     { 
      while(!digitalRead(KeyCol2));
      delay(10);
      return KEY5;
     }
      if(!digitalRead(KeyCol3)) 
     { 
      while(!digitalRead(KeyCol3));
      delay(10);
      return KEY6;
     }
    
     digitalWrite(KeyRowB,HIGH);
     digitalWrite(KeyRowC,LOW);


      if(!digitalRead(KeyCol1)) 
     { 
      while(!digitalRead(KeyCol1));
      delay(10);
      return KEY7;
     }
     if(!digitalRead(KeyCol2)) 
     { 
      while(!digitalRead(KeyCol2));
      delay(10);
      return KEY8;
     }
      if(!digitalRead(KeyCol3)) 
     { 
      while(!digitalRead(KeyCol3));
      delay(10);
      return KEY9;
     }
    
     digitalWrite(KeyRowC,HIGH);
     digitalWrite(KeyRowD,LOW);

       if(!digitalRead(KeyCol1)) 
     { 
      while(!digitalRead(KeyCol1));
      delay(10);
      return KEYSTAR;
     }
     if(!digitalRead(KeyCol2)) 
     { 
      while(!digitalRead(KeyCol2));
      delay(10);
      return KEY0;
     }
      if(!digitalRead(KeyCol3)) 
     { 
      while(!digitalRead(KeyCol3));
      delay(10);
      return KEYHASH;
     }
     
     
     digitalWrite(KeyRowD,HIGH);
     return NOKEY;
    
    }



void DisplayMain()
{
  LCDDisplay.clear();
  LCDDisplay.noCursor();
  LCDDisplay.noBlink();
  LCDDisplay.print("  -SMART HOME- ");
  if(ArlarmStatus==ACTIVE) LCDDisplay.print("!");
  LCDDisplay.setCursor(0,1);
  LCDDisplay.print("*:CAI MK|#:KHOA");
}


void DisplayStartUp()
{
  int i;
  LCDDisplay.clear();
  LCDDisplay.noCursor();
  LCDDisplay.noBlink();
  LCDDisplay.print("DANG KHOI DONG..");
  
  LCDDisplay.setCursor(0,1);
  for(i=0;i<16;i++)
  {
    LCDDisplay.print(">");
    delay(4000);
  }
  
}

void DisplayFunction(byte StateDisplay)
{
  LCDDisplay.clear();
  switch(StateDisplay)
  {
  
    case STACHANGEPASS:
    {

      LCDDisplay.setCursor(3,0);
      LCDDisplay.print("NHAP MA MOI"); 
      LCDDisplay.setCursor(3,1);
      CurrentPassIndex=0;
      break;
    }

    case STAUNLOCKBYCODE:
    {
       LCDDisplay.setCursor(3,0);
       LCDDisplay.print("DA MO CUA!"); 
       LCDDisplay.setCursor(3,1);
       LCDDisplay.print("XIN MOI VAO");
       OpenDoor();
       ArlarmStatus=INACTIVE;
       delay(3000);
       CurrentState=STAMAIN;
       DisplayMain();
       CloseDoor();
       break;
    }
    
    case STASETUPARLARM:
    {
      if(ArlarmStatus==ACTIVE)
      {
        ArlarmStatus=INACTIVE;
        LCDDisplay.setCursor(0,0);
        LCDDisplay.print("DA TAT BAO TROM!");
      }
      else
      {
        ArlarmStatus=ACTIVE;
        LCDDisplay.setCursor(0,0);
        LCDDisplay.print("DA BAT BAO TROM!");
      }
      delay(1000);
      CurrentState=STAMAIN;
      DisplayMain();
      break;
    }
    
  }
  
}

void OpenDoor()
{
  DoorServo.attach(DOOSERVO);
  DoorServo.write(120);
  delay(1000);
  DoorServo.detach();
}

void CloseDoor()
{
  DoorServo.attach(DOOSERVO);
  DoorServo.write(0);
  delay(1000);
  DoorServo.detach();
}


void CheckPIRSensor()
{
  if(digitalRead(PIRSENSOR)==HIGH)
  {
      if(ArlarmStatus==ACTIVE)
      {
        if(PIRArlarmActive==INACTIVE)
        {
           OnBuzzer();
           SendDataToESP(ESPCOMMAND,ARLARMSERCURITY);
           PIRArlarmActive=ACTIVE;
           delay(1000);
        }
        
      }
      
  }
  else
  {
    OffBuzzer();
    PIRArlarmActive=INACTIVE;
    if(FIREArlarmActive==INACTIVE&&GASArlarmActive==INACTIVE&&PIRArlarmActive==INACTIVE) OffBuzzer();
  }
}

void CheckFIRESensor()
{
  if(digitalRead(FIRESENSOR)==LOW)
  {
      if(FIREArlarmActive==INACTIVE)
      {
        OnBuzzer();
        FIREArlarmActive=ACTIVE;
        SendDataToESP(ESPCOMMAND,ARLARMFIRE);
        delay(1000);
      }
  }
  else
  {
      OffBuzzer();
      FIREArlarmActive=INACTIVE;
      if(FIREArlarmActive==INACTIVE&&GASArlarmActive==INACTIVE&&PIRArlarmActive==INACTIVE) OffBuzzer();
  }
}

void CheckGASSensor()
{
  if(digitalRead(GASSENSOR)==LOW)
  {
      if(GASArlarmActive==INACTIVE)
      {
        OnBuzzer();
        GASArlarmActive=ACTIVE;
        SendDataToESP(ESPCOMMAND,ARLARMGAS);
        delay(1000);
      }
  }
  else
  {
      
      GASArlarmActive=INACTIVE;
      if(FIREArlarmActive==INACTIVE&&GASArlarmActive==INACTIVE&&PIRArlarmActive==INACTIVE) OffBuzzer();
  }
}

void CheckButtonAnalog()
{
   if(analogRead(BUTTONANALOG)<1000) // found button trigger
  {
      delay(100); // wait for button stable
      int value = analogRead(BUTTONANALOG) + SmallestGap/2;
      for (int i=0; i<=6; i++)
      {
        if (value >= ButtonAnalogVals[i]) 
        {
         if(i>0) 
         {
          delay(200);
          SendDataToESP(ESPCOMMAND,ButtonCommand[i]);
         }
         return;
        }
      }
  }

}

void OnBuzzer()
{
  digitalWrite(BUZZER,HIGH);
}

void OffBuzzer()
{
  digitalWrite(BUZZER,LOW);
}


void setup() {
  // put your setup code here, to run once:

 pinMode(PIRSENSOR,INPUT);
 pinMode(FIRESENSOR,INPUT);
 pinMode(BUTTONANALOG,INPUT);
 pinMode(GASSENSOR,INPUT);
 pinMode(KeyRowA,OUTPUT);
 pinMode(KeyRowB,OUTPUT);
 pinMode(KeyRowC,OUTPUT);
 pinMode(KeyRowD,OUTPUT);

 pinMode(KeyCol1,INPUT_PULLUP);
 pinMode(KeyCol2,INPUT_PULLUP);
 pinMode(KeyCol3,INPUT_PULLUP);

 pinMode(BUZZER,OUTPUT);
 
 OffBuzzer();
 CloseDoor();
 
 Serial.begin(9600);
 DataSerial.begin(9600); //Open Serital to ESP8266 communication
 LCDDisplay.init();
 LCDDisplay.backlight();
 
 DisplayStartUp();
 
 DisplayMain();
 CurrentState=STAMAIN;
 GetPasswordFromEEPROM();
 
}

void loop() {
  // put your main code here, to run repeatedly:
  byte KeyCode;

   CheckPIRSensor();
   CheckFIRESensor();
   CheckButtonAnalog();
   CheckGASSensor();
   
   KeyCode=ScanKey();
   if(KeyCode!=NOKEY)
   {

      switch(CurrentState)
      {
        case STAMAIN:
        {
            switch(KeyCode)
            {
                case KEYSTAR:
                {
                  CurrentState=STACHECKPASS;
                  NextState=STACHANGEPASS;
                  DisPlayCheckPass();
                  break;
                }    
                case KEY0:
                case KEY1:
                case KEY2:
                case KEY3:
                case KEY4:
                case KEY5:
                case KEY6:
                case KEY7:
                case KEY8:
                case KEY9:
                 {
                  CurrentState=STACHECKPASS;
                  NextState=STAUNLOCKBYCODE;
                  DisPlayCheckPass();
                  break;
                }
                case KEYHASH:
                {
                  CurrentState=STACHECKPASS;
                  NextState=STASETUPARLARM;
                  DisPlayCheckPass();
                  break;
                }
                      
            }
         break;
        }
        
        
        case STACHECKPASS:
      {
        
           switch(KeyCode)
           {
              case KEY0:
              case KEY1:
              case KEY2:
              case KEY3:
              case KEY4:
              case KEY5:
              case KEY6:
              case KEY7:
              case KEY8:
              case KEY9:
              {
                if(CurrentPassIndex<6)// max is 6 digit
                {
                  CurrentPass[CurrentPassIndex]=KeyCode;
                  //LCDDisplay.write(CurrentPass[CurrentPassIndex]+48);
                  LCDDisplay.print("*");
                  CurrentPassIndex++;
                }
                
                break;
              }
              case KEYHASH:
              {
                
                if(VerifyPass()==true) 
                {
                  CurrentState=NextState;
                  DisplayFunction(CurrentState);
                  
                }
                else
                {
                   LCDDisplay.setCursor(3,1);
                   LCDDisplay.print("SAI MAT MA!  ");
                   delay(1000);
                   CurrentPassIndex=0;
                   CurrentPass[0]=0;
                   CurrentPass[1]=0;
                   CurrentPass[2]=0;
                   CurrentPass[3]=0;
                   CurrentPass[4]=0;
                   CurrentPass[5]=0;
                   LCDDisplay.setCursor(3,1);
                   LCDDisplay.print("             ");
                   LCDDisplay.setCursor(3,1);
                }
                break;
              }
               case KEYSTAR:
              {
                   CurrentPassIndex=0;
                   CurrentPass[0]=0;
                   CurrentPass[1]=0;
                   CurrentPass[2]=0;
                   CurrentPass[3]=0;
                   CurrentPass[4]=0;
                   CurrentPass[5]=0;
                   CurrentState=STAMAIN;
                   DisplayMain();
              }
            
           }
        
         break;
       }
       case STACHANGEPASS:
       {
        switch(KeyCode)
           {
              case KEY0:
              case KEY1:
              case KEY2:
              case KEY3:
              case KEY4:
              case KEY5:
              case KEY6:
              case KEY7:
              case KEY8:
              case KEY9:
              {
                if(CurrentPassIndex<6)// max is 6 digit
                {
                  CurrentPass[CurrentPassIndex]=KeyCode;
                  //LCDDisplay.write(CurrentPass[CurrentPassIndex]+48);
                   LCDDisplay.print("*");
                   CurrentPassIndex++;
                }
                
                break;
              }
              case KEYHASH:
              {
                
                if(CurrentPassIndex<6) 
                {
                  LCDDisplay.setCursor(3,1);
                  LCDDisplay.print("CHUA DU 6 KT");
                   delay(1000);
                   CurrentPassIndex=0;
                   CurrentPass[0]=0;
                   CurrentPass[1]=0;
                   CurrentPass[2]=0;
                   CurrentPass[3]=0;
                   CurrentPass[4]=0;
                   CurrentPass[5]=0;
                   LCDDisplay.setCursor(3,1);
                   LCDDisplay.print("             ");
                   LCDDisplay.setCursor(3,1);
                }
                else
                {
                   PassWord[0]=CurrentPass[0];
                   PassWord[1]=CurrentPass[1];
                   PassWord[2]=CurrentPass[2];
                   PassWord[3]=CurrentPass[3];
                   PassWord[4]=CurrentPass[4];
                   PassWord[5]=CurrentPass[5];
                   SavePasswordToEEPROM();
                   LCDDisplay.setCursor(3,1);
                   LCDDisplay.print("DA LUU MA MOI");
                   
                   delay(1000);
                   CurrentPassIndex=0;
                   CurrentPass[0]=0;
                   CurrentPass[1]=0;
                   CurrentPass[2]=0;
                   CurrentPass[3]=0;
                   CurrentPass[4]=0;
                   CurrentPass[5]=0;
                   CurrentState=STAMAIN;
                   DisplayMain();
                }
                break;
              }
               case KEYSTAR:
              {
                   CurrentPassIndex=0;
                   CurrentPass[0]=0;
                   CurrentPass[1]=0;
                   CurrentPass[2]=0;
                   CurrentPass[3]=0;
                   CurrentPass[4]=0;
                   CurrentPass[5]=0;
                   CurrentState=STAMAIN;
                   DisplayMain();
                   break;
              }
            
           }
        break;
      }
     }
   }

}
