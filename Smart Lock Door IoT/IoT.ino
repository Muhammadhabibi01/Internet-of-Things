/* Read RFID Tag with RC522 RFID Reader
    Made by miliohm.com
*/
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>
#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>

#define RST_PIN  D3    // Configurable, see typical pin layout above
#define SS_PIN  D4    // Configurable, see typical pin layout 

#define FIREBASE_HOST "iot4sc6-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "IUM2KLFBIvUKoNEzCtSqIHHPHQt7EY2JKLyoubvH"
#define WIFI_SSID "OPPO K3"
#define WIFI_PASSWORD "qwerty1010"


#define TouchSensor 4
FirebaseData firebaseData;
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;
Servo kunci;
int PIR = 16;
int state = LOW;
int pos = 0;
String tag;
boolean currentState = LOW;
boolean lastState = LOW;
boolean RelayState = LOW;


void setup() {
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  
  //3. Set your Firebase info

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  //4. Enable auto reconnect the WiFi when connection lost
  Firebase.reconnectWiFi(true);

  if(Firebase.setInt(firebaseData, "/STATUS/STATUS_PINTU/Int", 2))
  {
    //Success
     Serial.println("Set int data success");

  }else{
    //Failed?, get the error reason from firebaseData

    Serial.print("Error in setInt, ");
    Serial.println(firebaseData.errorReason());
  }
  pinMode(16, INPUT);
  Serial.begin(115200);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
  pinMode(D8, OUTPUT);
  kunci.attach(5); //servo pin D1
  kunci.write(0);
  pinMode(4, INPUT);
}




void kontrol(){
     if(Firebase.getInt(firebaseData, "/STATUS/STATUS_PINTU/Int"))
  {
    //Success
    Serial.print("Get int data success, int = ");
    delay(500);
    Serial.println(firebaseData.intData());

  }else{
    //Failed?, get the error reason from firebaseData
    digitalWrite(D8,LOW);
    Serial.print("Error in getInt, ");
    Serial.println(firebaseData.errorReason());
  }
  int x =firebaseData.intData();
  if ( x == 1){
    kunci.write(180);
    pos = 1;
     digitalWrite(D8,HIGH);
     Firebase.setString(firebaseData, "/STATUS/STATUS_PINTU/Value", "(ON)Mobile-Access");
     Firebase.setString(firebaseData, "/STATUS/STATUS_PINTU/History", "UNLOCKED");
     Firebase.setString(firebaseData,"/STATUS/STATUS_PINTU/INFORMATION", "Rumah Anda Telah Diakses Melalui Mobile Aplication (Android)");
     delay(1000);
  }else if (x == 0){
    Firebase.setString(firebaseData, "/STATUS/STATUS_PINTU/Value", "(OFF)Mobile-Access");
    Firebase.setString(firebaseData, "/STATUS/STATUS_PINTU/History", "LOCKED");
    Firebase.setString(firebaseData,"/STATUS/STATUS_PINTU/INFORMATION", "Rumah Anda Telah Diakses Melalui Mobile Aplication (Android)");
    kunci.write(0);
    pos = 0;
    digitalWrite(D8,LOW);
  }
}

void pir2(){
  int val = digitalRead(PIR);
  
  if(val == 1){
    digitalWrite(D8, HIGH);
    Serial.println("               | Pergerakan Terdeteksi |               ");
    delay(200);
    digitalWrite(D8,LOW);
    delay(200);
    digitalWrite(D8, HIGH);
    Serial.println("               |   W A R N I N G !!!   |               ");
    delay(200);
    digitalWrite(D8,LOW);
    delay(200);
    digitalWrite(D8, HIGH);
    Serial.println("               |   W A R N I N G !!!   |               ");
    delay(200);
    digitalWrite(D8,LOW);
    kunci.write(0);
    pos = 0;
    Serial.println("           | Semua Telah Terkunci dan Aman |             ");
        if(Firebase.setString(firebaseData, "/STATUS/PIR/Value", "Warning ! ! !"))
      {
        Firebase.setString(firebaseData,"/STATUS/PIR/History", "Warning ! ! !");
         Firebase.setString(firebaseData,"/STATUS/PIR/INFORMATION", "Mendeteksi Adanya Pergerakan Mencurigakan,Jika Bukan Anda Segera Hubungi Polisi");
         digitalWrite(D8,HIGH);
         delay(6000);
         digitalWrite(D8,LOW);
         Serial.println("Set int data success");
         delay(2000); // DISINI UNTUK MENGATUR BERAPA LAMA HISTORY DAPAT TERUPDATE======================<<<<<>>>>>>>>
         Firebase.setString(firebaseData,"/STATUS/PIR/History", "Refresh (24 Jam)");
      }else{
        Serial.print("Error in setInt, ");
        Serial.println(firebaseData.errorReason());
          }
  }else{
    digitalWrite(D8, LOW);
      if(Firebase.setString(firebaseData, "/STATUS/PIR/Value", "Aman"))
      {
        Firebase.setString(firebaseData,"/STATUS/PIR/INFORMATION", "Rumah Anda Dalam Kondisi Aman");
        Serial.println("Tidak Ada Pergerakan");
      }else{
        Serial.print("Error in setInt, ");
        Serial.println(firebaseData.errorReason());
      }
  }
}

void touch_on(){
      currentState = digitalRead(TouchSensor);
  if (currentState == HIGH && lastState == LOW){
    if(pos == 0){
      digitalWrite(D8, HIGH);
      delay(100);
      digitalWrite(D8, LOW);
      delay(100);
      digitalWrite(D8, HIGH);
      delay(100);
      digitalWrite(D8, LOW);
    kunci.write(180);
    pos = 1;
    Serial.println("                          ||                      ");
    Serial.println("                          ||                      ");
    Serial.println("               | Pintu Telah Terbuka |            ");
    Serial.println("            | Pintu Akan Terkunci Otomatis |            ");
    digitalWrite(D8, HIGH);
    delay(2500);
    digitalWrite(D8, LOW);
    delay(100);
    digitalWrite(D8, HIGH);
    delay(100);
    digitalWrite(D8, LOW);
    delay(100);
    digitalWrite(D8, HIGH);
    delay(100);
    digitalWrite(D8, LOW);    
    kunci.write(0);
    pos = 0;
    Serial.println("                          ||                      ");
    Serial.println("                          ||                      ");
    Serial.println("               | Pintu Telah Terkunci |           ");    
    }
    else if(pos > 0){
      digitalWrite(D8, HIGH);
      delay(100);
      digitalWrite(D8, LOW);
      delay(100);
      digitalWrite(D8, HIGH);
      delay(100);
      digitalWrite(D8, LOW);
      delay(100);
      digitalWrite(D8, HIGH);
      delay(100);
      digitalWrite(D8, LOW);
     kunci.write(0);
     pos = 0;
     Serial.println("                          ||                      ");
     Serial.println("                          ||                      ");
     Serial.println("               | Pintu Telah Terkunci |           ");
    }
  }
}

void nfc(){
  if ( ! rfid.PICC_IsNewCardPresent())
    return;
  if (rfid.PICC_ReadCardSerial()) {
    for (byte i = 0; i < 4; i++) {
      tag += rfid.uid.uidByte[i];
    }
//    Serial.println(tag);
    if (tag=="997912112"){
      digitalWrite(D8, HIGH);
      delay(100);
      digitalWrite(D8, LOW);
      delay(100);
      digitalWrite(D8, HIGH);
      delay(100);
      digitalWrite(D8, LOW);
      Serial.println("                | Pintu Telah Terbuka |               ");
      Serial.println("                |     Kode Anda       |               ");
      Serial.println("       ==============="+ tag +"===================    ");
      if(Firebase.setString(firebaseData, "/STATUS/RFID/Value", "Mr.Habibi"))
      {
      Serial.println("Pintu Diakses Oleh Habibi");
      Firebase.setString(firebaseData,"/STATUS/RFID/History", "Diakses");
      Firebase.setString(firebaseData,"/STATUS/RFID/INFORMATION", "Mendeteksi Kartu Dengan Hak-Akses Telah Masuk");
      kunci.write(180);
      pos = 1;
//      delay(3000);

      }else{

      Serial.print("Error in setInt, ");
      Serial.println(firebaseData.errorReason());
      }
  }else{
    digitalWrite(D8, HIGH);
    delay(300);
    Serial.println("                | Maaf Akses Ditolak |               ");
    Serial.println("                |     Kode Anda       |               ");
    Serial.println("       ==============="+ tag +"===================    ");
    if(Firebase.setString(firebaseData, "/STATUS/RFID/Value","No Akses")) //konsul,cara membuat Warning 1,2,3,.... setiap kartu No Akses di scan
    {
    Firebase.setString(firebaseData,"/STATUS/RFID/History", "No Akses");
    Serial.println("Sorry....");
    Firebase.setString(firebaseData,"/STATUS/RFID/INFORMATION", "Mendeteksi Adanya Kartu Tanpa Hak-Akses yang Mencoba Untuk Masuk");
    }else{

    Serial.print("Error in setInt, ");
    Serial.println(firebaseData.errorReason());
    }
  }
    tag = "";
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}

void loop() {
//  deteksi();
  touch_on();
  kontrol();
  nfc();
  pir2();
  
}
