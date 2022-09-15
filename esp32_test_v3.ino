//#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>
//#include <FreeRTOS.h>
//#include <Adafruit_BME280.h>
//#include <Adafruit_Sensor.h>


#include "DHT.h"

#define DHTPIN 23     // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.
int AOUT_PIN = 35;
int rain_pin = 34;
int light_pin = 39;
int gas_pin = 36;
int servo_pin = 13;
// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
#define INTERVAL_MESSAGE1 5000
#define INTERVAL_MESSAGE2 7000
#define INTERVAL_MESSAGE3 11000
#define INTERVAL_MESSAGE4 13000
// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

Servo myservo;




// variable for storing the potentiometer value
int potValue = 0;
int pos = 180;

const char *SSID = "HACKATHON";
const char *PWD = "H@ck!2022t0n";

const int a_1 = 21;   
const int a_2 = 19; 
const int b_1 = 18; 
const int b_2 = 5;

// Setting PWM frequency, channels and bit resolution
const int frequency = 5000;
const int forwardChannel = 0;
const int leftChannel = 1;
const int rightChannel = 2;
const int backwardChannel = 3;
const int resolution = 8;

WebServer server(80);
 
//Adafruit_BME280 bme;

StaticJsonDocument<250> jsonDocument;
char buffer[250];

float temp_air;
float hum_air;
float hum_soil;
float light;
float rain;
float gas;
float btemp_air = 25.3;
float bhum_air = 53;

int servo_case;
int relay;

void setup_routing() {     
  server.on("/general", get_temp_air);     
  server.on("/control", transmit);     
  /*server.on("/hum_soil", get_hum_soil);     
  server.on("/light", get_light); 
  server.on("/rain", get_rain); 
  server.on("/gas", get_gas);*/    
  server.on("/motor", HTTP_POST, handlePost);    
          
  server.begin();    
}
 
void create_json(char *temp, float value, char *hum1, float value1, char *hum2, float value2, char *rain, float value3, char *light, float value4, char *gas, float value5) {  
  jsonDocument.clear();  
  jsonDocument[temp] = value;
  jsonDocument[hum1] = value1;
  jsonDocument[hum2] = value2;
  jsonDocument[rain] = value3;
  jsonDocument[light] = value4;
  jsonDocument[gas] = value5;
  serializeJson(jsonDocument, buffer);
}

void create_json_1(char *window, int servo_case, char *pump, int relay) {  
  jsonDocument.clear();  
  jsonDocument[window] = servo_case;
  jsonDocument[pump] = relay;
  serializeJson(jsonDocument, buffer);
}
 
void add_json_object(char *temp, float value, char *hum1, float value1, char *hum2, float value2, char *rain, float value3, char *light, float value4, char *gas, float value5) {
  JsonObject obj = jsonDocument.createNestedObject();
  obj[temp] = value;
  obj[hum1] = value1;
  obj[hum2] = value2;
  obj[rain] = value3;
  obj[light] = value4;
  obj[gas] = value5; 
}


void add_json_object_1(char *window, int servo_case, char *pump, int relay){
  JsonObject obj = jsonDocument.createNestedObject();
  
  //Serial.print(temp_air);
  Serial.println(servo_case);
  obj[window] = servo_case;
  obj[pump] = relay;
  
}




void read_sensor_data(void * parameter) {

  
  
   for (;;) {
    
     // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
 hum_air = dht.readHumidity();
  // Read temperature as Celsius (the default)
  temp_air = dht.readTemperature();
  if(isnan(temp_air)){
    temp_air = btemp_air;}
    if(isnan(hum_air)){
    hum_air = bhum_air;}
  int value = analogRead(AOUT_PIN); // read the analog value from sensor
      hum_soil = map(value,0,4095,100,0);
      
  
  
  rain = map(analogRead(rain_pin),0,4095,0,100);
  light = map(analogRead(light_pin),0,4095,0,100);
  
  gas = map(analogRead(gas_pin),0,4095,0,100);

  //Serial.println(light);
  //Serial.println(rain);
  Serial.println(temp_air);
  // Check if any reads failed and exit early (to try again).
 /* if (isnan(hum_air) || isnan(temp_air) ) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }*/

  
 
 
     vTaskDelay(2000 / portTICK_PERIOD_MS);
     //time_1 = 0;
   }
}
 
void get_temp_air() {
  Serial.println("Get temperature");
  create_json("temp_air", temp_air, "hum_air", hum_air,"hum_soil",hum_soil,"rain",rain,"light", light,"gas", gas);
  server.send(200, "application/json", buffer);
}
 
void transmit() {
  Serial.println("transmitting data");
  create_json_1("window", servo_case, "pump", relay);
  server.send(200, "application/json", buffer);
}

/*void get_hum_soil() {
  Serial.println("Get humidity");
  create_json("hum_soil", hum_soil, "%");
  server.send(200, "application/json", buffer);
}

void get_light() {
  Serial.println("Get humidity");
  create_json("light", light, "%");
  server.send(200, "application/json", buffer);
}

void get_rain() {
  Serial.println("Get humidity");
  create_json("rain", rain, "%");
  server.send(200, "application/json", buffer);
}
 
void get_gas() {
  Serial.println("Get pressure");
  create_json("gas", gas, "hPa");
  server.send(200, "application/json", buffer);
}*/
 
void getData() {
  Serial.println("Get BME280 Sensor Data");
  jsonDocument.clear();
  add_json_object("temp_air", temp_air, "hum_air", hum_air,"hum_soil",hum_soil,"rain",rain,"light", light,"gas", gas);
  /*add_json_object("hum_air", hum_air, "%");
  add_json_object("hum_soil", hum_soil, "%");
  add_json_object("light", light, "%");
  add_json_object("rain", rain, "%");
  add_json_object("gas", gas, "%");*/
  serializeJson(jsonDocument, buffer);
  server.send(200, "application/json", buffer);
}

void handlePost() {
  if (server.hasArg("plain") == false) {
  }
  String body = server.arg("plain");
  deserializeJson(jsonDocument, body);

  int forward = jsonDocument["forward"];
  int left = jsonDocument["left"];
  int right = jsonDocument["right"];
  int backward = jsonDocument["backward"];
  Serial.print(forward);
  Serial.print("  ");
  Serial.print(left);
  Serial.print("  ");
  Serial.print(right);
  Serial.print("  ");
  Serial.println(backward);
  
  if(forward == 1){
    forward_go();
  }
  else if(left == 1){
    left_go();
  }
  else if(right == 1){
    right_go();
  }
  else if(backward == 1){
    backward_go();
  }
  else{
    stop_go();
  }
  /*ledcWrite(forwardChannel, red_value);
  ledcWrite(leftChannel,green_value);
  ledcWrite(rightChannel, blue_value);
  ledcWrite(backwardChannel, red_value);*/

  server.send(200, "application/json", "{}");
}



void setup_task() {    
  xTaskCreate(     
  read_sensor_data,      
  "Read sensor data",      
  1000,      
  NULL,      
  1,  
  NULL     
  );     
}

//int period_1 = {4000,500,2000,500,4000,500,2000,500,4000,500,2000,500,4000}
//unsigned long time_1,time_2,time_3,time_4,time_5,time_6,time_7, time_8, time_9, time_10, time_11, time_12, time_13 = 0;
//int period_1 = 4000;

//unsigned long time_2 = 0;

void setup() { 
  myservo.write(pos);
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservo.attach(servo_pin, 500, 2400);
  
  pinMode(a_1, OUTPUT); 
  pinMode(a_2, OUTPUT);
  pinMode(b_1, OUTPUT);
  pinMode(b_2, OUTPUT);
  
  adcAttachPin(4);
    
  
  Serial.println(F("DHTxx test!"));

  dht.begin();   
  Serial.begin(115200); 

  /*ledcSetup(redChannel, frequency, resolution);
  ledcSetup(greenChannel, frequency, resolution);
  ledcSetup(blueChannel, frequency, resolution);
 
  ledcAttachPin(red_pin, redChannel);
  ledcAttachPin(green_pin, greenChannel);
  ledcAttachPin(blue_pin, blueChannel);*/
         
//  if (!bme.begin(0x76)) {    
  //  Serial.println("BME280 not found! Check Circuit");    
  //}    

  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(SSID, PWD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
 
  Serial.print("Connected! IP Address: ");
  Serial.println(WiFi.localIP());
  setup_task();    
  setup_routing();     
   
}    

int period = 4000;
unsigned long time_now = 0;
int period_1 = 4000;
unsigned long time_now_2 = 0;

 
unsigned long time_1 = 0;
unsigned long time_2 = 0;
unsigned long time_3 = 0;
unsigned long time_4 = 0;
 
void print_time(unsigned long time_millis);
 

 

    
 
void print_time(unsigned long time_millis){
    Serial.print("Time: ");
    Serial.print(time_millis/1000);
    Serial.print("s - ");
}





      
void loop() { 
  if(millis() >= time_1 + INTERVAL_MESSAGE1){
        time_1 +=INTERVAL_MESSAGE1;
        print_time(time_1);
        
        if(pos == 180){
          pos = 90;
          myservo.write(pos);
        }else{
          pos = 180;
          myservo.write(pos);
        }
    }
   
    if(millis() >= time_2 + INTERVAL_MESSAGE2){
        time_2 +=INTERVAL_MESSAGE2;
        print_time(time_2);
        Serial.println("Hello, I'm the second message.");
    }
   
    if(millis() >= time_3 + INTERVAL_MESSAGE3){
        time_3 +=INTERVAL_MESSAGE3;
        print_time(time_3);
        Serial.println("My name is Message the third.");
    }
   
    if(millis() >= time_4 + INTERVAL_MESSAGE4){
        time_4 += INTERVAL_MESSAGE4;
        print_time(time_4);
        Serial.println("Message four is in the house!");
    }
   
  server.handleClient();
  delay(100);
  if(hum_air > 70.0){
    servo_case = 1;
    
  }else{
    servo_case = 0;
  }
  if(hum_soil > 70.0){
    relay = 1;
  }else{
    relay = 0;
  }
  time_now = millis();
   time_now_2 = millis();
    
   
    
  
  
  
  
  delay(50);    
  
  
  //soil_hum();
  //delay(500);
  //autonomous();
  time_now = 0;
}



/*void autonomous(){
   time_1 = millis();
   time_2 = millis();
   
    
   
  while(millis() < time_1 + period_1){
    digitalWrite(a_1, HIGH);
    digitalWrite(a_2, LOW);
    }
    //stop_go();
    //delay(50);
  while(millis() < time_2 + period_2){
    forward_go();
  }

  Serial.println(time_1);
}*/
 void forward_go(){
  digitalWrite(a_2, HIGH);
  digitalWrite(a_1, LOW);
  digitalWrite(b_2, HIGH);
  digitalWrite(b_1, LOW);
  
}
void backward_go(){
  digitalWrite(a_1, HIGH);
  digitalWrite(a_2, LOW);
  digitalWrite(b_1, HIGH);
  digitalWrite(b_2, LOW);
  
}
void left_go(){
  digitalWrite(a_1, LOW);
  digitalWrite(a_2, HIGH);
  digitalWrite(b_1, HIGH);
  digitalWrite(b_2, LOW);
  
}
void right_go(){
  digitalWrite(a_1, HIGH);
  digitalWrite(a_2, LOW);
  digitalWrite(b_1, LOW);
  digitalWrite(b_2, HIGH);
  
  
}

void stop_go(){
  digitalWrite(a_1, LOW);
  digitalWrite(a_2, LOW);
  digitalWrite(b_1, LOW);
  digitalWrite(b_2, LOW);
}

/*void soil_hum(){
  while(millis() < time_1 + period_1){
      myservo.write(90);
      int value = analogRead(AOUT_PIN); // read the analog value from sensor
      hum_soil = map(value,0,4095,100,0);
      delay(200);
      myservo.write(180);
    }
}*/
