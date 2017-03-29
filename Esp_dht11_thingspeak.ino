#include <SoftwareSerial.h>
#include <stdlib.h>
#include <dht.h>

#define DHT11_PIN 7
dht DHT;

int ledPin = 13;  //LED
int lm35Pin = 0;  // LM35 analog input

String apiKey = "your thingspeak API key";  // replace with your channel's thingspeak API key

// connect 10 to TX of Serial USB
// connect 11 to RX of serial USB
SoftwareSerial ser(10, 11); // RX, TX


void setup() {                
  pinMode(ledPin, OUTPUT);    // initialize the digital pin as an output.
  Serial.begin(9600);    // enable debug serial
  ser.begin(9600);     // enable software serial
  ser.println("AT+RST");   // reset ESP8266
}


void loop() {
  digitalWrite(ledPin, HIGH);    // blink LED on board
  delay(200);               
  digitalWrite(ledPin, LOW);

  // read the value from DHT11.
  int chk = DHT.read11(DHT11_PIN);
  Serial.print("Temperature = ");
  Serial.println(DHT.temperature);
  Serial.print("Humidity = ");
  Serial.println(DHT.humidity);
  delay(5000);
  float temp = DHT.temperature;

  // convert to string
  char buf[16];
  String strTemp = dtostrf(temp, 4, 1, buf);

  Serial.println(strTemp);

  // TCP connection
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // api.thingspeak.com
  cmd += "\",80";
  ser.println(cmd);

  if(ser.find("Error")){
    Serial.println("AT+CIPSTART error");
    return;
  }

  String getStr = "GET /update?api_key=";    // prepare GET string
  getStr += apiKey;
  getStr +="&field1=";
  getStr += String(strTemp);
  getStr += "\r\n\r\n";
  
  cmd = "AT+CIPSEND=";   // send data length
  cmd += String(getStr.length());
  ser.println(cmd);

  if(ser.find(">")){
    ser.print(getStr);
  }
  else{
    ser.println("AT+CIPCLOSE");
    
    Serial.println("AT+CIPCLOSE");// alert user
  }

  delay(16000);    // thingspeak needs 15 sec delay between updates
}
