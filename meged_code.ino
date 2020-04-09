#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <SoftwareSerial.h>
SoftwareSerial sim808(D7,D8);

/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

String api_key = "xxxxxxxxxxxxxxxxx"; //  ThingSpeak API Key
String data[5];
#define DEBUG true
String state,timegps,latitude,longitude;
float field1,field2,field3;
void setup() {
  /* Initialise the sensor */
 if(!accel.begin())
 {
 /* There was a problem detecting the ADXL345 ... check your connections */
 Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
 while(1);
 }
 
 /* Set the range to whatever is appropriate for your project */
 accel.setRange(ADXL345_RANGE_16_G);
 // displaySetRange(ADXL345_RANGE_8_G);
 // displaySetRange(ADXL345_RANGE_4_G);
 // displaySetRange(ADXL345_RANGE_2_G);
 
 sim808.begin(9600);
 Serial.begin(115200);
 delay(50);

 sim808.print("AT+CSMP=17,167,0,0");  // set this parameter if empty SMS received
 delay(100);
 sim808.print("AT+CMGF=1\r"); 
 delay(400);

 sendData("AT+CGNSPWR=1",1000,DEBUG);
 delay(50);
 sendData("AT+CGNSSEQ=RMC",1000,DEBUG);
 delay(150);
 
}

void loop() {
  /* Get a new sensor event */ 
 sensors_event_t event; 
 accel.getEvent(&event);
 field1 = event.acceleration.x;
 field2 = event.acceleration.y;
 field3 = event.acceleration.z;
  /* Display the results (acceleration is measured in m/s^2) */
 Serial.print("X: "); Serial.print(field1); Serial.print(" ");
 Serial.print("Y: "); Serial.print(field2); Serial.print(" ");
 Serial.print("Z: "); Serial.print(field3); Serial.print(" ");Serial.println("m/s^2 ");
 delay(500);
 
 sendTabData("AT+CGNSINF",1000,DEBUG);
  if (state !=0) {
    Serial.println("State  :"+state);
    Serial.println("Time  :"+timegps);
    Serial.println("Latitude  :"+latitude);
    Serial.println("Longitude  :"+longitude);
    sim808.flush();
    uploadData(field1, field2, field3, latitude, longitude);
    sim808.flush();
  } else {
    Serial.println("GPS Initialising...");
  }
}

void sendTabData(String command , const int timeout , boolean debug){

  sim808.println(command);
  long int time = millis();
  int i = 0;

  while((time+timeout) > millis()){
    while(sim808.available()){
      char c = sim808.read();
      if (c != ',') {
         data[i] +=c;
         delay(100);
      } else {
        i++;  
      }
      if (i == 5) {
        delay(100);
        goto exitL;
      }
    }
  }exitL:
  if (debug) {
    state = data[1];
    timegps = data[2];
    latitude = data[3];
    longitude =data[4];  
  }
}
String sendData (String command , const int timeout ,boolean debug){
  String response = "";
  sim808.println(command);
  long int time = millis();
  int i = 0;

  while ( (time+timeout ) > millis()){
    while (sim808.available()){
      char c = sim808.read();
      response +=c;
    }
  }
  if (debug) {
     Serial.print(response);
     }
     return response;
}

void uploadData(float field1, float field2, float field3, String latitude, String longitude)       //send data with gsm
{
  sim808.println("AT"); //check gsm
  delay(1000);

  sim808.println("AT+CPIN?"); //Check if sim locked with pin
  delay(1000);

  sim808.println("AT+CREG?"); //Check if gets registered to sim network
  delay(1000);

  sim808.println("AT+CGATT?"); //Check if gets attached to registered network
  delay(1000);

  sim808.println("AT+CIPSHUT"); //Close any previous active PDP (Packet data protocol) context
  delay(1000);

  sim808.println("AT+CIPSTATUS"); //Check current connection status (bearer, TCP/UDP, IP, Port etc.)
  delay(2000);

  sim808.println("AT+CIPMUX=0"); //Deactive any multi IP connections
  delay(2000);
 
  ShowSerialData();
 
  sim808.println("AT+CSTT=\"internet\"");//start task and setting the APN,
  delay(1000);
 
  ShowSerialData();
 
  sim808.println("AT+CIICR");//bring up wireless connection
  delay(3000);
 
  ShowSerialData();
 
  sim808.println("AT+CIFSR");//get local IP adress
  delay(2000);
 
  ShowSerialData();
 
  sim808.println("AT+CIPSPRT=0"); //Set prompt if module is avail to send data
  delay(3000);
 
  ShowSerialData();
  
  sim808.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");//start up the connection
  delay(6000);
 
  ShowSerialData();
 
  sim808.println("AT+CIPSEND");//begin send data to remote server
  delay(4000);
  ShowSerialData();
  
  String str="GET https://api.thingspeak.com/update?api_key="  + api_key 
  +"&field1="+ String(field1) 
  +"&field2="+ String(field2) 
  +"&field3="+ String(field3)
  +"&field4="+ latitude 
  +"&field5="+ longitude; //thinkspeak data writing url
  
  sim808.println(str);//begin send data to remote server
  delay(4000);
  ShowSerialData();

  sim808.println((char)26);//sending
  delay(5000);//waiting for reply, important! the time is base on the condition of internet 
  sim808.println();
 
  ShowSerialData();
 
  sim808.println("AT+CIPSHUT");//close the connection
  delay(100);
  ShowSerialData();
}

void ShowSerialData() //Show serial data
{
  while(sim808.available()!=0)
    Serial.write(sim808.read());
    Serial.println();
}
