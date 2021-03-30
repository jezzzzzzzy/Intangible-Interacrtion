/*  This is a sketch that access the ITP Weather Station's database on weatherband.itp.io
 *  and get weather data one by one, starting from id #1.
 *  You'll need to parse the Json file to use specific values.
 *  
 *  April 2020: Modified by Yeseul Song & Arnab Chakravarty 
 *  November 2020: Modified by Yeseul Song for the ITP Weather Band, with the wip new database
 *  Based on Tom Igoe's Connected Devices example
*/
 #include <ArduinoJson.h>
#include <ArduinoHttpClient.h>
#include <WiFiNINA.h> // you might want to use WiFi101.h instead depending on which arduino you're using

#include "arduino_secrets.h" 

int ledPin = 2;
int dataLED = 0;

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

const char serverAddress[] = "weatherband.itp.io";  // server address
int port = 3000;  // for https
int counter = 1;

WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);
int status = WL_IDLE_STATUS;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  
  while (!Serial);
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);     // print the network name (SSID);

    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
  }

  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

void loop() {  
  
  getData();

  // read the status code and body of the response
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  parseJson(response);

//  Serial.print("Status code: ");
//  Serial.println(statusCode);
//  Serial.print("Response: ");
  //Serial.println(response);
    


  delay(100);
}

void getData() {

  Serial.println("making GET request");
  String path = "http://weatherband.itp.io:3000/data/id/1";
  path+= counter;
  
  client.get(path);
  
  counter++;
  
}

void parseJson(String response) {
  
  const size_t capacity = JSON_OBJECT_SIZE(19) + 270; // calculated at https://arduinojson.org/v6/assistant/
  DynamicJsonDocument doc(capacity);

  deserializeJson(doc, response);
  DeserializationError error = deserializeJson(doc, response);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  //int id = doc["id"]; // e.g., 10
  const char* mac_address = doc["mac_address"]; // e.g., "A4:CF:12:8A:C8:24"
  const char* recorded_at = doc["recorded_at"]; // e.g., "2020-11-13T01:59:15.000Z"
  //int wind_dir = doc["wind_dir"]; // e.g., 248
  //int winddir_avg2m = doc["winddir_avg2m"]; // e.g., 0
//  int windspeedmph = doc["windspeedmph"]; // e.g., 0
//  int windgustdir_10m = doc["windgustdir_10m"]; // e.g., 0
//  int windgustmph_10m = doc["windgustmph_10m"]; // e.g., 0
//  int rainin = doc["rainin"]; // e.g., 0
//  int dailyrainin = doc["dailyrainin"]; // e.g, 0
//  float temperature = doc["temperature"]; // e.g., 30.03
//  float humidity = doc["humidity"]; // e.g., 115.37
//  float pressure = doc["pressure"]; // e.g., 101.84
  float illuminance = doc["illuminance"]; // e.g., 17.74
//  int uva = doc["uva"]; // 0
//  int uvb = doc["uvb"]; // 0
//  int uvindex = doc["uvindex"]; // 0

  Serial.print("Weather data recorded at ");
  Serial.print(recorded_at);
  Serial.println(" has been saved to the weather variables and they're ready to use.");

  Serial.println(illuminance);

  
  int i = illuminance *100;
   dataLED = map(i, 0, 1000, 0, 255);
   analogWrite(ledPin, dataLED);

  
}
