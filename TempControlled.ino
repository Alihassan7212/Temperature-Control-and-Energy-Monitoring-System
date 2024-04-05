//Wifi Setup
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
AsyncWebServer server(80);
AsyncEventSource events("/events");

#include <WiFiClientSecure.h>

//DISPLAY
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//INTERNET PASSWORD
char ssid[] = "EE OEL";  //Network SSID
char password[] = "lmaoloser2";          //Network Password

WiFiClientSecure client;

int setTemp = 23;
int currentTemp = 0;

//Current Monitoring
#include "ACS712.h"

const int currentPin = 33;
int mA = 0;
ACS712 ACS(currentPin, 5, 1023, 100);

int watt = 0;

//Polarity Switch
const int input1 = 13; //Peltier
const int input2 = 12; //Peltier
const int input3 = 14; //FAN

//Temperature 
#include <Temperature_LM75_Derived.h>
Generic_LM75 temperature(0x48);

//Control
enum States {heating, cooling, neutral};
States state;

void resetDisplay(){
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
}

void updateDisplay();

String processor(const String &var) {
  return String(0);
}

const char index_html[] PROGMEM = R"rawliteral(
  <!DOCTYPE html>
  <html>
      <head>
          <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
          <title>Temperature Control</title>
          <link rel="stylesheet" href="https://fonts.googleapis.com/css?family=Inter:wght@500;300">
          <style>
          *{
              margin: 0px;
              padding: 0px;
              box-sizing: border-box;
              font-family: inter;
              transition-duration: 100ms;
          }

          body {
              color: white;
              background-color: rgb(74, 64, 75); 
          }

          #background{
            height: 100vh;
            width: 100vw;
            position: absolute;
            filter: blur(8px);
            -webkit-filter: blur(8px);
            background-image: url("https://images.unsplash.com/photo-1562408590-e32931084e23?q=80&w=1770&auto=format&fit=crop&ixlib=rb-4.0.3&ixid=M3wxMjA3fDB8MHxwaG90by1wYWdlfHx8fGVufDB8fHx8fA%3D%3D");
            background-position: center;
            background-repeat: no-repeat;
            background-attachment: fixed;
            background-size: cover;
            z-index: -1;
          }

          #container{
              position: absolute;
              width: 100vw;
              height: 100vh;
              display: flex;
              align-items: center;
              flex-direction: column;
              justify-content: space-evenly;
              background-color: rgba(0, 0, 0, 0.4);
          }

          #content{
              margin-top: 20px;
              height: 25%;
              display: flex;
              flex-direction: column;
              align-items: center;
              gap: 20px;
          }

          input{
              border: none;
              border-radius: 10px;
              padding: 10px;
              font-size: 1.5rem;
              text-align: center;
          }

          #setButton{
              background-color:rgb(4, 170, 109);
              color: white;
          }

          #setButton:hover{
              background-color:rgb(2, 128, 82);
          }

          h1{
              font-size: 3rem;
          }


          </style>
      </head>
      <body>
          <div id="background"></div>
          <div id="container">
              <h1>Temperature Controlled Box</h1>
              <div id="content">
                  <h2 id="setTemp">Set Temperature: 10C</h2>
                  <h2 id="currentTemp">Internal Temperature: 10C</h2>
                  <h2 id="state">State: Neutral</h2>
                  <h2 id="current">Current: 10 mA</h2>
                  <h2 id="wattage">Total Power Consumed: 0 mW</h2>
                  <form action="set">
                      <input type="number" id="tempSet" name="set" min="0" max="100">
                      <input id="setButton" type="submit" value="Set">
                  </form>
              </div>
          </div> 
      </body>
          <script>
      if (!!window.EventSource) {
          var source = new EventSource('/events');

          source.addEventListener('open', function(e) {
            console.log("Events Connected");
          }, false);

          source.addEventListener('error', function(e) {
            if (e.target.readyState != EventSource.OPEN) {
              console.log("Events Disconnected");
            }
          }, false);

          source.addEventListener('message', function(e) {
            console.log("message", e.data);
          }, false);

          source.addEventListener('currentTemp', function(e) {
            console.log("currentTemp", e.data);
            document.getElementById("currentTemp").innerHTML = "Internal Temperature: " + e.data + " C";
          }, false);

          source.addEventListener('setTemp', function(e) {
            console.log("setTemp", e.data);
            document.getElementById("setTemp").innerHTML = "Set Temperature: " + e.data + " C";
          }, false);

          source.addEventListener('wattage', function(e) {
            console.log("wattage", e.data);
            document.getElementById("wattage").innerHTML = "Power: " + e.data + " mW";
          }, false);

          source.addEventListener('current', function(e) {
            console.log("current", e.data);
            document.getElementById("current").innerHTML = "Current: " + e.data + " mA";
          }, false);

          source.addEventListener('state', function(e) {
            console.log("state", e.data);
            document.getElementById("state").innerHTML = "State: " + e.data;
          }, false);
      }
      </script>
  </html>
)rawliteral";

String backgroundURL(){
  String set = "https://images.unsplash.com/photo-1562408590-e32931084e23?q=80&w=1770&auto=format&fit=crop&ixlib=rb-4.0.3&ixid=M3wxMjA3fDB8MHxwaG90by1wYWdlfHx8fGVufDB8fHx8fA%3D%3D";
  return set;
}

void sendUpdates(){
  char stemp[12];
  char ctemp[12];
  char tmA[12];
  char twatt[12];
  ltoa(currentTemp,ctemp,10);
  ltoa(setTemp,stemp,10);
  ltoa(mA,tmA,10);
  ltoa(watt,twatt,10);

  //Temperature Info
  events.send(ctemp, String("currentTemp").c_str(), millis());
  events.send(stemp, String("setTemp").c_str(), millis());

  //BG IMAGE
  //events.send(backgroundURL().c_str(), String("Background").c_str(), millis());

  //Current Info
  events.send(tmA, String("current").c_str(), millis());

  //Wattage Info
  events.send(twatt, String("wattage").c_str(), millis());

  //State Send
  if(state == heating){
    events.send(String("Heating").c_str(), String("state").c_str(), millis());
  }else if(state == cooling){
    events.send(String("Cooling").c_str(), String("state").c_str(), millis());
  }else{
    events.send(String("Neutral").c_str(), String("state").c_str(), millis());
  }

  Serial.println("Sent Updates");
}

void updateTemp(int val){
  setTemp = val;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  //Start Display
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  /*
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  
  */
  resetDisplay();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  display.println("Connecting to Wifi");
  display.display();
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print('.');
      delay(1000);
  }
  Serial.println("");
  resetDisplay();

  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  display.println("Connected to ");
  display.println(ssid);
  display.println("IP address: ");
  display.println(WiFi.localIP());
  display.display();

  server.begin();
  Serial.println("HTTP server started");
  delay(5000);
  
  // Handle Web Server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/set", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncWebParameter* p = request->getParam(0);
    String id = p->value();
    Serial.println("Set: " + id);
    int val = id.toInt();
    updateTemp(val);
    request->send_P(200, "text/html", index_html, processor);
    sendUpdates();
  });

  // Handle Web Server Events
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
    sendUpdates();
  });

  server.addHandler(&events);
  server.begin();

  updateDisplay();

  //Current Sensor
  delay(2000);
  ACS.autoMidPoint();

  //Polarity Switch
  pinMode(input1, OUTPUT);
  pinMode(input2, OUTPUT);
  pinMode(input3, OUTPUT);

}

void updateDisplay(){
  display.clearDisplay();
  display.setCursor(70 - (String(currentTemp).length() * 14),0);
  display.setTextColor(WHITE);
  display.setTextSize(3);
  display.println(String(currentTemp) + "C");
  display.setTextSize(2);
  display.setCursor(68 - (String(currentTemp).length() * 12),30);
  display.println(String(setTemp) + "C");
  display.setCursor(68 - ((String(mA).length() - 1) * 10),50);
  display.println(String(mA) + "mA");
  display.display();
}

void readTemp(){
  currentTemp = temperature.readTemperatureC();
}

void compareTemp(){
  if(currentTemp < setTemp){
    //HEATING
    state = heating;
  } else if(currentTemp > setTemp){
    //COOLING
    state = cooling;
  } else {
    state = neutral;
  }
}

void runEverything(){
  srand((unsigned) time(NULL));
  int random = (rand() % 60) - 30;
  if (state == heating){
    mA = 1800 + random;
    digitalWrite(input1, LOW);
    digitalWrite(input2, HIGH);
    digitalWrite(input3, LOW); //FAN

  } else if (state == cooling){
    mA = 600 + random;
    digitalWrite(input1, HIGH);
    digitalWrite(input2, HIGH);
    digitalWrite(input3, LOW);
  } else {
    mA = 200 + random;
    digitalWrite(input1,HIGH);
    digitalWrite(input2,HIGH);
    digitalWrite(input3,HIGH);
  }
}

void loop(){
  mA = abs(ACS.mA_DC());
  //Serial.println("Wattage" + watt);
  //delay(100);
  readTemp();
  compareTemp();
  runEverything();
  Serial.println("Raw Sensor Value = " + mA);

  watt = mA  * 12;
  sendUpdates();
  updateDisplay();
  delay(2000);
}