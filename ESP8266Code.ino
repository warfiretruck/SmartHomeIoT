#define BLYNK_TEMPLATE_ID "TMPLPaXXXJ17"
#define BLYNK_TEMPLATE_NAME "SMARTHOME"
#define BLYNK_AUTH_TOKEN "RW4KVQCL4oj2_8cXLrgw--h8WnRw0KD_"

#include <Arduino.h>
#include <PCF8574.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>  
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <DHT_U.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SRX D5 
#define STX D6

#define I2CSDA D2
#define I2CSCL D1

#define DHTPIN D7
#define DHTTYPE DHT11

#define BLYNK_DEBUG
#define BLYNK_PRINT Serial
#define SW_PIN 0

#define OFFLIGHTROOM1 MyPCFRelay.digitalWrite(P0, HIGH)
#define ONLIGHTROOM1 MyPCFRelay.digitalWrite(P0, LOW)
#define OFFFANROOM1 MyPCFRelay.digitalWrite(P1, HIGH)
#define ONFANROOM1 MyPCFRelay.digitalWrite(P1, LOW)
#define OFFLIGHTROOM2 MyPCFRelay.digitalWrite(P2, HIGH)
#define ONLIGHTROOM2 MyPCFRelay.digitalWrite(P2, LOW)
#define OFFFANROOM2 MyPCFRelay.digitalWrite(P4, HIGH)
#define ONFANROOM2 MyPCFRelay.digitalWrite(P4, LOW)
#define OFFLIGHTROOM3 MyPCFRelay.digitalWrite(P5, HIGH)
#define ONLIGHTROOM3 MyPCFRelay.digitalWrite(P5, LOW)
#define OFFFANROOM3 MyPCFRelay.digitalWrite(P6, HIGH)
#define ONFANROOM3 MyPCFRelay.digitalWrite(P6, LOW)




#define ON 0
#define OFF 1

#define TURNONLIGHT1 0x01
#define TURNONFAN1 0x02
#define TURNOFFLIGHT1 0x03
#define TURNOFFFAN1 0x04
#define TURNONLIGHT2 0x05
#define TURNONFAN2 0x06
#define TURNOFFLIGHT2 0x07
#define TURNOFFFAN2 0x08
#define TURNONLIGHT3 0x09
#define TURNONFAN3 0x0A
#define TURNOFFLIGHT3 0x0B
#define TURNOFFFAN3 0x0C


#define ARLARMFIRE 0xA4
#define ARLARMSERCURITY 0xA5
#define ARLARMGAS 0xA6

#define BUTTONLIGHT1 0xA8
#define BUTTONFAN1 0xA9
#define BUTTONLIGHT2 0xAA
#define BUTTONFAN2 0xAB
#define BUTTONLIGHT3 0xAC
#define BUTTONFAN3 0xAD

#define ESPCOMMAND 0xB1
#define ESPHUMIDATA 0xB2
#define ESPTEMPDATA 0xB3


char blynk_token[] = BLYNK_AUTH_TOKEN;
//char blynk_token[] ="";
SoftwareSerial DataSerial(SRX, STX);
//#define DataSerial Serial


byte UARTByteCount;
byte UARTCommand;
byte UARTParameter;
byte PCFData=0;

bool LightRoom1=OFF;
bool FanRoom1=OFF;
bool LightRoom2=OFF;
bool FanRoom2=OFF;
bool LightRoom3=OFF;
bool FanRoom3=OFF;


bool LastStateLightRoom1=OFF;
bool LastStateFanRoom1=OFF;
bool LastStateLightRoom2=OFF;
bool LastStateFanRoom2=OFF;
bool LastStateLightRoom3=OFF;
bool LastStateFanRoom3=OFF;



BlynkTimer ClockTimer;
BlynkTimer WritePCFRelayTimer;
BlynkTimer UARTTimer;
BlynkTimer BlynkReconnectTimer;

PCF8574 MyPCFRelay(0x27);
LiquidCrystal_I2C MyLCD(0x26, 16, 2);
DHT TempHumSensor = DHT(DHTPIN , DHTTYPE);

float CurrentTemperature;
float CurrentHumidity;

BLYNK_WRITE(V0)
{
  int PinValue = param.asInt();
  if(PinValue==0) LightRoom1=ON;
  else LightRoom1=OFF;

}

BLYNK_WRITE(V1)
{
  int PinValue = param.asInt();
  if(PinValue==0) FanRoom1=ON;
  else FanRoom1=OFF;
}

BLYNK_WRITE(V2)
{
  int PinValue = param.asInt();
  if(PinValue==0) LightRoom2=ON;
  else LightRoom2=OFF;
}

BLYNK_WRITE(V3)
{
  int PinValue = param.asInt();
  if(PinValue==0) FanRoom2=ON;
  else FanRoom2=OFF;
}

BLYNK_WRITE(V4)
{
  int PinValue = param.asInt();
  if(PinValue==0) LightRoom3=ON;
  else LightRoom3=OFF;
}

BLYNK_WRITE(V5)
{
  int PinValue = param.asInt();
  if(PinValue==0) FanRoom3=ON;
  else FanRoom3=OFF;
}

void WritePCFRelay()
{
 if(LightRoom1!=LastStateLightRoom1)
 {
  LastStateLightRoom1=LightRoom1;
  Blynk.virtualWrite(V0,LightRoom1); // please remember to set on Blynk app ON is 0 OFF is 1
  if(LightRoom1==ON) 
  {
    ONLIGHTROOM1;
  }
  else 
  {
    OFFLIGHTROOM1;
  }
 }

 if(FanRoom1!=LastStateFanRoom1)
 {
  LastStateFanRoom1=FanRoom1;
  Blynk.virtualWrite(V1,FanRoom1); // please remember to set on Blynk app ON is 0 OFF is 1
  if(FanRoom1==ON) 
  {
    ONFANROOM1;
  }
  else 
  {
    OFFFANROOM1;
  }
 }

 if(LightRoom2!=LastStateLightRoom2)
 {
  LastStateLightRoom2=LightRoom2;
  Blynk.virtualWrite(V2,LightRoom2); // please remember to set on Blynk app ON is 0 OFF is 1
  if(LightRoom2==ON) 
  {
    ONLIGHTROOM2;
  }
  else 
  {
    OFFLIGHTROOM2;
  }
 }

 if(FanRoom2!=LastStateFanRoom2)
 {
  LastStateFanRoom2=FanRoom2;
  Blynk.virtualWrite(V3,FanRoom2); // please remember to set on Blynk app ON is 0 OFF is 1
   if(FanRoom2==ON) 
  {
    ONFANROOM2;
  }
  else 
  {
    OFFFANROOM2;
  }
 }

  if(LightRoom3!=LastStateLightRoom3)
 {
  LastStateLightRoom3=LightRoom3;
  Blynk.virtualWrite(V4,LightRoom3); // please remember to set on Blynk app ON is 0 OFF is 1
  if(LightRoom3==ON) 
  {
    ONLIGHTROOM3;
  }
  else 
  {
    OFFLIGHTROOM3;
  }
 }

 if(FanRoom3!=LastStateFanRoom3)
 {
  LastStateFanRoom3=FanRoom3;
  Blynk.virtualWrite(V5,FanRoom3); // please remember to set on Blynk app ON is 0 OFF is 1
  if(FanRoom3==ON) 
  {
    ONFANROOM3;
  }
  else 
  {
    OFFFANROOM3;
  }
 }
 

}

void UARTRead()
{
   byte UARTDataCome;
   if (DataSerial.available()>0)
   {
      UARTDataCome=DataSerial.read();
      if(UARTByteCount==0)
      {
        UARTCommand=UARTDataCome;
      }

      if(UARTByteCount==1&&UARTCommand==ESPCOMMAND)
        {
              switch(UARTDataCome)
                 {
                          case BUTTONLIGHT1:
                          {
                            if(LightRoom1==ON) LightRoom1=OFF;
                            else LightRoom1=ON;
                            break;
                          }
                           case BUTTONFAN1:
                          {
                            if(FanRoom1==ON) FanRoom1=OFF;
                            else FanRoom1=ON;
                            break;
                          }
                          case BUTTONLIGHT2:
                          {
                            if(LightRoom2==ON) LightRoom2=OFF;
                            else LightRoom2=ON;
                            break;
                          }
                            case BUTTONFAN2:
                          {
                            if(FanRoom2==ON) FanRoom2=OFF;
                            else FanRoom2=ON;
                            break;
                          }
                          case BUTTONLIGHT3:
                          {
                            if(LightRoom3==ON) LightRoom3=OFF;
                            else LightRoom3=ON;
                            break;
                          }
                            case BUTTONFAN3:
                          {
                            if(FanRoom3==ON) FanRoom3=OFF;
                            else FanRoom3=ON;
                            break;
                          }
                  
                         

                          case ARLARMFIRE:
                          {
                            Blynk.logEvent("EVENT2","CẢNH BÁO PHÁT HIỆN LỬA!!!");
                            break;
                          }
                  
                           case ARLARMSERCURITY:
                          {
                           Blynk.logEvent("EVENT1","CANH BAO - NHA BAN CO TROM!!!");
                            break;
                          }
                  
                          case ARLARMGAS:
                          {
                            Blynk.logEvent("EVENT3","CANH BAO - NHA BAN RO RI KHI GAS!!!");
                            break;
                          }
                   }
          UARTByteCount=0;
          UARTCommand=0;
        }
        else
        {
          UARTByteCount++;
          if(UARTByteCount>5) UARTByteCount=0; //prevent accidentcount
        }
     }     
}


 void GetTempHumi()
 {
   CurrentHumidity = TempHumSensor.readHumidity();
   CurrentTemperature = TempHumSensor.readTemperature();
 }

 void DisplayTempHumi()
{
  char DataString[4];
  GetTempHumi();
  if (isnan(CurrentHumidity) || isnan(CurrentTemperature) ||CurrentHumidity>100) //skip if read fail
  {
     Serial.println("Failed to read from DHT sensor!");
     return;
     }
  MyLCD.setCursor(9,1);
  dtostrf(CurrentHumidity,2,0,DataString);
  MyLCD.print(DataString);
  MyLCD.print("% ");
  MyLCD.setCursor(9,0);
  dtostrf(CurrentTemperature,4,1,DataString);
  MyLCD.print(DataString);
  MyLCD.print("oC");
   Blynk.virtualWrite(V6,(int)CurrentHumidity);
   Blynk.virtualWrite(V7,CurrentTemperature);
  
}



void BlynkReconnect()
{
  if(Blynk.connected()==0)
  {
    Blynk.connect(3333);
  }
}

void DisplayMain()
{
  MyLCD.clear();
  MyLCD.print("Nhiet do:");
  MyLCD.setCursor(0,1);
  MyLCD.print("   Do Am:");
}

void setup() {
  Serial.begin(9600);                    // Thiết lập kết nối Serial để truyền dữ liệu đến máy tính
  DataSerial.begin(9600); // thiet lan ket noi voi arduino
  pinMode(SW_PIN,INPUT_PULLUP); // Nut tren board bam de xoa wifi da luu
  UARTByteCount=0;


  TempHumSensor.begin();
  MyLCD.init();
  MyLCD.backlight();

  MyLCD.setCursor(0, 0); 
  MyLCD.print(" HT SMART HOME  ");
  MyLCD.setCursor(0, 1); 
  MyLCD.print("BAM R RESET WIFI");

  
  
  WiFiManager wifiManager;                 // Khởi tạo đối tượng cho WiFiManager
  Serial.println("Delete old wifi? Press Flash button within 3 second");
  for(int i=3;i>0;i--)
  {
    Serial.print(String(i)+" ");
    delay(1000);
      }
  if(digitalRead(SW_PIN)==LOW)// press button
  {
   Serial.println();
   Serial.println("Reset wifi config!");
   MyLCD.setCursor(0, 1);
   MyLCD.print("-> 192.168.4.1  ");
   wifiManager.resetSettings();   
   
  }
   WiFiManagerParameter custom_blynk_token("Blynk", "blynk token", blynk_token, 34);
   wifiManager.addParameter(&custom_blynk_token);
   wifiManager.autoConnect("SMARTHOME","QWER1234");
   //wifiManager.autoConnect();// use this to display host as ESP name + CHIPID
  // if can go next mean already connected wifi
  Serial.println("YOU ARE CONNECTED TO WIFI");
  Serial.println(custom_blynk_token.getValue());
  Blynk.config(custom_blynk_token.getValue());
  if (WiFi.status() == WL_CONNECTED) 
  {
   Serial.println("WIFI CONNECTED SUCCESSFULLY! NOW TRY TO CONNECT BLYNK..."); 
   MyLCD.setCursor(0, 1);
   MyLCD.print("DA KET NOI WIFI ");
   delay(1000); 
   Blynk.connect(3333); // try to connect to Blynk with time out 10 second
   if(Blynk.connected()) 
   {
    Serial.println("BLYNK CONNECTED! System ready"); 
    MyLCD.setCursor(0, 1);
    MyLCD.print("DA KET NOI BLYNK");
     delay(1000);
   }
   else 
   {
    Serial.println(" BLYNK Not connect. warning!");
    MyLCD.setCursor(0, 1);
    MyLCD.print("LOI K.NOI BLYNK!");
     delay(1000);
   }
  }
  else 
  {
    Serial.println("WIFI Not connect. warning!");
    MyLCD.setCursor(0, 1);
    MyLCD.print("LOI K.NOI WIFI! ");
    delay(1000);
  }
  //MAIN CODE FOR SETUP
   DisplayMain();
  WritePCFRelayTimer.setInterval(10L,WritePCFRelay);
  UARTTimer.setInterval(1L,UARTRead);
  BlynkReconnectTimer.setInterval(1000L,BlynkReconnect);
  ClockTimer.setInterval(1000L,DisplayTempHumi);;

  
  MyPCFRelay.pinMode(P0, OUTPUT);
  MyPCFRelay.pinMode(P1, OUTPUT);
  MyPCFRelay.pinMode(P2, OUTPUT);
  MyPCFRelay.pinMode(P4, OUTPUT);
  MyPCFRelay.pinMode(P5, OUTPUT);
  MyPCFRelay.pinMode(P6, OUTPUT);
  MyPCFRelay.pinMode(P7, OUTPUT);
  MyPCFRelay.begin();
  OFFLIGHTROOM1;
  OFFFANROOM1;
  OFFLIGHTROOM2;
  OFFFANROOM2;
  OFFLIGHTROOM3;
  OFFFANROOM3;

  

}

void loop() {
  Blynk.run();
  WritePCFRelayTimer.run();
  UARTTimer.run();
  BlynkReconnectTimer.run();
  ClockTimer.run();
  //UARTRead();

  // put your main code here, to run repeatedly:

}
