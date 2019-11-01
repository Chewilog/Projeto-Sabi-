#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <stdlib.h>

#define SERVICE_UUID        "20ff121c-94e1-11e9-bc42-526af7764f64"
#define CHARACTERISTIC_UUID "20ff156e-94e1-11e9-bc42-526af7764f64"
//Matriz de leds
int Ac=32;
int Bc=33;
int Al=21;
int Bl=22;
int Cl=23;
int pos;
int pins[] = {Ac,Bc,Al,Bl,Cl};
//BLE
bool deviceConnected = false;
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
//Buzzer
int resolution = 10;
int freq = 987;
int channel = 15;
int bPin = 4;
float nFreq[] = {294,277,262,247,233,220,207,196,440,415,392,370,349,330,311,294,659,622,587,554,523,494,466,440,988,932,880,831,784,740,698,659};



int defPos(std::string c)
{
  int pos = 0;
  pos += c[1]-48;
  pos += (c[0]-48)*10;
 
  return pos;
}
int defTime(std::string a)
{
      int b = 0;
      b += a[5]-48;
      b += (a[4]-48)*10;
      b += (a[3]-48)*100;
      b += (a[2]-48)*1000;
      
      return b;
}

void dell()
{
 for(int i=0;i<5;i++)
 {
  digitalWrite(pins[i],LOW);
 } 
}

void ligaCol(int pos)
{
  if (pos/8 == 0)
  {
   digitalWrite(Ac,LOW);
   digitalWrite(Bc,LOW);
  }
  if(pos/8 == 1)
  {
    digitalWrite(Ac, HIGH);
    digitalWrite(Bc, LOW);
  }
  if(pos/8 == 2)
  {
    digitalWrite(Ac,LOW);
    digitalWrite(Bc, HIGH);
  }
  if(pos/8 == 3)
  {
    digitalWrite(Ac, HIGH);
    digitalWrite(Bc, HIGH);
  }
}

void ligaRow(int pos)
{
  if (pos%8 == 7)
  {
    digitalWrite(Al,LOW);
    digitalWrite(Bl,LOW);
    digitalWrite(Cl,LOW);
  }
  if (pos%8 == 6)
  {
    digitalWrite(Al,HIGH);
    digitalWrite(Bl,LOW);
    digitalWrite(Cl,LOW); 
  }
  if (pos%8 == 5)
  {
    digitalWrite(Al,LOW);
    digitalWrite(Bl,HIGH);
    digitalWrite(Cl,LOW); 
  }
  if (pos%8 == 4)
  {
    digitalWrite(Al,HIGH);
    digitalWrite(Bl,HIGH);
    digitalWrite(Cl,LOW); 
  }
  if (pos%8 == 3)
  {
    digitalWrite(Al,LOW);
    digitalWrite(Bl,LOW);
    digitalWrite(Cl,HIGH); 
  }
  if (pos%8 == 2)
  {
    digitalWrite(Al,HIGH);
    digitalWrite(Bl,LOW);
    digitalWrite(Cl,HIGH); 
  }
  if (pos%8 == 1)
  {
    digitalWrite(Al,LOW);
    digitalWrite(Bl,HIGH);
    digitalWrite(Cl,HIGH); 
  }
  if (pos%8 == 0)
  {
    digitalWrite(Al,HIGH);
    digitalWrite(Bl,HIGH);
    digitalWrite(Cl,HIGH); 
  }
}


class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      ledcWriteTone(channel, 0);
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
       ligaCol(0);
       ligaRow(0);
    }
};



class MyCallbacks: public BLECharacteristicCallbacks {
    
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() == 6) {
 
        pos = defPos(rxValue);
        ligaCol(pos);
        ligaRow(pos);
        ledcWriteTone(channel,nFreq[pos]);
        delay(defTime(rxValue));
        ledcWriteTone(channel,0);
//        dell();
        delay(3);
        pCharacteristic->setValue("33");
        
      }
      else
      {
        delay(10);
        ligaCol(defPos(0));
        ligaRow(defPos(0));
      }
    }
};

void setup() {
  //Matriz de leds
  pinMode(Ac,OUTPUT);
  pinMode(Bc,OUTPUT);
  pinMode(Al,OUTPUT);
  pinMode(Bl,OUTPUT);
  pinMode(Cl,OUTPUT);
  pinMode(25,OUTPUT);
  digitalWrite(25,LOW);
  dell();
  //Buzzer
  ledcSetup(channel,2000,resolution);
  ledcAttachPin(4, channel);
  ledcWriteTone(channel,0);
  //BLE
  BLEDevice::init("EME");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE|
                                         BLECharacteristic::PROPERTY_NOTIFY
                                       );
                                       
  pCharacteristic->setCallbacks(new MyCallbacks());                                  
  pCharacteristic->setValue("32");
  pService-> start();                                     

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(3);
}
