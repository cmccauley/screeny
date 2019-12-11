//
// A simple server implementation showing how to:
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
//


#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

// #include <Wire.h>
// #include <LiquidCrystal.h>

// #define   RS     5 // GPIO5 D1
// #define   ENABLE 4 // GPIO4 D2
// #define   DATA1  14 // GPIO14 D5
// #define   DATA2  12 // GPIO12 D6
// #define   DATA3  13 // GPIO13 D7
// #define   DATA4  15 // GPIO15 D8

//LiquidCrystal lcd(RS, ENABLE, DATA1, DATA2, DATA3, DATA4);


AsyncWebServer server(80);

//void writeToLcd(String message);

const char* ssid = "Shopify";
const char* password = "tophat made from dollar bills";

const char* PARAM_MESSAGE = "message";

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void setup() {

    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.printf("WiFi Failed!\n");
        return;
    }

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        Serial.print("request yolo swag");
        request->send(200, "text/plain", "Hello, world");
    });

    // Send a GET request to <IP>/get?message=<message>
    server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
                Serial.print("request yolo swag getttt");

        String message;
        if (request->hasParam(PARAM_MESSAGE)) {
            message = request->getParam(PARAM_MESSAGE)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, GET: " + message);
    });

    // Send a POST request to <IP>/post with a form field message set to <message>
    server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
                Serial.print("request yolo postttt");

        String message;
        if (request->hasParam(PARAM_MESSAGE, true)) {
            message = request->getParam(PARAM_MESSAGE, true)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, POST: " + message);
    });

    server.onNotFound(notFound);
            Serial.print("begin yo");

    server.begin();
        Serial.print("begin end");

    //lcd.begin(16,2);               // initialize the lcd 
    

}

// void writeToLcd(String message) {
//     lcd.home ();                   // go home
//     lcd.print("Hello, CHEMEX");  
//     lcd.setCursor ( 0, 1 );        // go to the next line
//     lcd.print (message);  
//     delay(1000);
// }

void loop() {
  //writeToLcd("yo");
  // put your main code here, to run repeatedly:

}
