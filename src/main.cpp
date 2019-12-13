//
// A simple server implementation showing how to:
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
// gateway 10.11.128.1


#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <Wire.h>
#include <LiquidCrystal.h>

#define   RS     5 // GPIO5 D1
#define   ENABLE 4 // GPIO4 D2
#define   DATA1  14 // GPIO14 D5
#define   DATA2  12 // GPIO12 D6
#define   DATA3  13 // GPIO13 D7
#define   DATA4  15 // GPIO15 D8

#define QUANTITY_ENDPOINT "https://chemecois-bot.herokuapp.com/quantity"

struct LCDData {
    int cups_left = 4;
    String line1 = "Starting";
    String line2 = "Great software";
};


LCDData data;
HTTPClient http_client;
LiquidCrystal lcd(RS, ENABLE, DATA1, DATA2, DATA3, DATA4);
AsyncWebServer server(80);

void writeToLcd(LCDData data);
void updatesCupLeft(LCDData data, int quantity);
void syncCupsWithSlack(LCDData data);
void writeToLcd(LCDData data);
void take();
void release();

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
        writeToLcd(data);
        request->send(200, "text/plain", "Hello, world");
    });

    // Send a GET request to <IP>/get?message=<message>
    server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String message;
        if (request->hasParam(PARAM_MESSAGE)) {
            message = request->getParam(PARAM_MESSAGE)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, GET: " + message);
    });

    // Send a POST request to <IP>/post with a form field message set to <message>
    server.on("/post", HTTP_GET, [](AsyncWebServerRequest *request){
        String message = "Error";

        Serial.println("Received a post!");

        if (request->hasParam("line1")) {
            message = request->getParam("line1")->value();
            data.line1 = message;
        }

        if (request->hasParam("line2")) {
            message = request->getParam("line2")->value();
            data.line2 = message;
        }

        writeToLcd(data);

        request->send(200, "text/plain", "Hello, POST: " + message);
    });

    server.on("/slack", HTTP_POST, [](AsyncWebServerRequest *request){
        String message = "Missing challenge";
        if (request->hasParam("challenge", true)) {
            message = request->getParam("challenge", true)->value();
            data.line2 = message;
            writeToLcd(data);
        }
        Serial.println("Received a post! with message" + message);
        request->send(200, "text/plain", message);
    });

    server.on("/release", HTTP_GET, [](AsyncWebServerRequest *request){
        String message;
        if (request->hasParam(PARAM_MESSAGE, true)) {
            message = request->getParam(PARAM_MESSAGE, true)->value();
            writeToLcd(data);
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, POST: " + message);
    });

    server.onNotFound(notFound);

    server.begin();

    lcd.begin(16, 2);               // initialize the lcd
    writeToLcd(data);
}

void writeToLcd(LCDData data) {
    lcd.clear();
    lcd.home ();                   // go home
    lcd.print(data.line1);
    lcd.setCursor (0, 1);        // go to the next line
    lcd.print (data.line2);
}

void take(LCDData data){
    data.cups_left--;
}

void release(LCDData data){
    data.cups_left++;
}

void syncCupsWithSlack(LCDData data) {
  http_client.begin(QUANTITY_ENDPOINT);
  int httpCode = http_client.GET();
  Serial.println(httpCode);

  StaticJsonDocument<200> jsonResponse;

  if (httpCode > 0) {
    Serial.println(http_client.getString());
    deserializeJson(jsonResponse, http_client.getString());
    int quantity = jsonResponse["quantity"];
    Serial.println(quantity);
    updatesCupLeft(data, quantity);
    data.line2 = "Cups left: " + data.cups_left;
    writeToLcd(data);
  }

  http_client.end();
}

void updatesCupLeft(LCDData data, int cup_lefts) {
  data.cups_left = cup_lefts;
}

void loop() {
  syncCupsWithSlack(data);
  delay(2000);
}
