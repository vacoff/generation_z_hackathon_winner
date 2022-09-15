#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>



const char *SSID = "HACKATHON";
const char *PWD = "H@ck!2022t0n";

Servo myservo;

int pos = 0;
int servo_pin = 13;
int relay_pin = 12;
// Setting PWM frequency, channels and bit resolution
const int frequency = 5000;
const int redChannel = 0;
const int greenChannel = 1;
const int blueChannel = 2;
const int resolution = 8;

WebServer server(80);
 
//Adafruit_BME280 bme;

StaticJsonDocument<250> jsonDocument;
char buffer[250];

float temperature;
float humidity;
float pressure;
float bruh;

int relay;
int servo_case;

void setup_routing() {     
  server.on("/temperature", getTemperature);        
  server.on("/control", HTTP_POST, handlePost);    
          
  server.begin();    
}
 
void create_json(char *tag, float value, char *unit) {  
  jsonDocument.clear();  
  jsonDocument["type"] = tag;
  jsonDocument["value"] = value;
  jsonDocument["unit"] = unit;
  serializeJson(jsonDocument, buffer);
}

void create_json_1(char *window, int servo_case, char *pump, int relay) {  
  jsonDocument.clear();  
  jsonDocument[window] = servo_case;
  jsonDocument[pump] = relay;
  serializeJson(jsonDocument, buffer);
}
 
void add_json_object(char *tag, float value, char *unit) {
  JsonObject obj = jsonDocument.createNestedObject();
  obj["type"] = tag;
  obj["value"] = value;
  obj["unit"] = unit; 
}

void add_json_object_1(char *window, int servo_case, char *pump, int relay){
  JsonObject obj = jsonDocument.createNestedObject();
  obj[window] = servo_case;
  obj[pump] = relay;
}

void read_sensor_data(void * parameter) {
   for (;;) {
     
     Serial.println("Read sensor data");
      
     vTaskDelay(2000 / portTICK_PERIOD_MS);
   }
}
 
void getTemperature() {
  Serial.println("Get temperature");
  create_json_1("window", servo_case, "pump", relay);
  server.send(200, "application/json", buffer);
}
 
/*void getHumidity() {
  Serial.println("Get humidity");
  create_json("humidity", humidity, "%");
  server.send(200, "application/json", buffer);
}
 
void getPressure() {
  Serial.println("Get pressure");
  create_json("pressure", pressure, "hPa");
  server.send(200, "application/json", buffer);
}

void getBruh() {
  Serial.println("Get pressure");
  create_json_1("pressure", bruh);
  server.send(200, "application/json", buffer);
}

 
void getData() {
  Serial.println("Get BME280 Sensor Data");
  jsonDocument.clear();
  add_json_object("temperature", temperature, "°C");
  add_json_object("humidity", humidity, "%");
  add_json_object("pressure", pressure, "hPa");
  serializeJson(jsonDocument, buffer);
  server.send(200, "application/json", buffer);
}

void getData_1() {
  Serial.println("Get BME280 Sensor Data");
  jsonDocument.clear();
  add_json_object("temperature", temperature, "°C");
  add_json_object("humidity", humidity, "%");
  //add_json_object("pressure", pressure, "hPa");
  serializeJson(jsonDocument, buffer);
  server.send(200, "application/json", buffer);
}*/

void handlePost() {
  if (server.hasArg("plain") == false) {
  }
  String body = server.arg("plain");
  deserializeJson(jsonDocument, body);

  servo_case = jsonDocument["window"];
  relay = jsonDocument["pump"];

  Serial.print(relay);
  Serial.println(servo_case);
  

  server.send(200, "application/json", "{}");
}

void handlePost_1(){
  String body = server.arg("plain");
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

void setup() {  
  pinMode(relay_pin, OUTPUT);  
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservo.attach(servo_pin, 500, 2400);
   
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
    delay(500);
  }
 
  Serial.print("Connected! IP Address: ");
  Serial.println(WiFi.localIP());
  setup_task();    
  setup_routing();     
   
}    
       
void loop() {    
  server.handleClient();
  if(servo_case == 1){
    myservo.write(90);
  }else if( servo_case == 0){
    myservo.write(0);
  }
  if(relay == 1){
    digitalWrite(relay_pin, HIGH);
  }else{
    digitalWrite(relay_pin, LOW);
  }
     
}
