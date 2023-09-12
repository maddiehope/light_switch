# include <WiFi.h> //Wifi library 
# include "esp_wpa2.h" //wpa2 library for connections to Enterprise networks
# include <M5StickCPlus.h>
// test
#include <Wire.h>

# define EAP_ANONYMOUS_IDENTITY ""
# define EAP_IDENTITY "mph63858@uga.edu"
# define EAP_PASSWORD "Rocky656428654278872!" //password for eduroam account 

//# define WPA_PASSWORD "hnetbricealison081002" //password for home wifi

#define USE_EAP

#ifdef USE_EAP
  const char* ssid = "eduroam"; // eduroam SSID
//#else 
  //const char* ssid = "hnet"; // home wifi SSID
#endif


//--------------------------------------------------------------------------
#include <ArduinoMqttClient.h>
const char broker[] = "info8000.ga";
int        port     = 1883;
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);
const char light_control_color[] = "ugaelee2045sp23/mph63858/light_control_color";
const char light_control_status[]= "ugaelee2045sp23/mph63858/light_control_status";
const char light_status[]= "ugaelee2045sp23/mph63858/light_status";
char buffer[100];


//--------------------------------------------------------------------------

// button settings
const int button = 37; // G37 on M5StickC

// variables for topics
int status = 0;
int r = 0;
int g = 0;
int b = 0;

// variables for the message conversion 
int this_r;
int this_b;
int this_g;

//--------------------------------------------------------------------------

uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b)
{
  return ((r/8) << 11) | ((g/4) << 5) | (b/8);
}

void updateStatus()
{
  M5.Axp.ScreenBreath(status?15:0);
  M5.Lcd.fillScreen(rgb565(r,g,b));
}

void sendStatus()
{
  mqttClient.beginMessage(light_status);
  mqttClient.write(status);
  mqttClient.write(r);
  mqttClient.write(g);
  mqttClient.write(b);
  mqttClient.endMessage();

}


void onMqttMessage(int messageSize)
{
  mqttClient.read((byte *)buffer,messageSize);
  Serial.println(buffer[0]);

  //int length = sizeof(buffer) / sizeof(buffer[0]);

  if(buffer[0] == '#') //characteristic indicator
  {
    Serial.println("color recieved");
    
    memcpy(&r, &buffer[1], sizeof(r));
    memcpy(&g, &buffer[2], sizeof(g));
    memcpy(&b, &buffer[3], sizeof(b));

    // Print the values
    /*
    Serial.print("r: ");
    Serial.println(r);
    Serial.print("g: ");
    Serial.println(g);
    Serial.print("b: ");
    Serial.println(b);
    */
    updateStatus();
  
  }
  else if (buffer[0] == '0' || buffer[0] == '1')
  {

    Serial.println("on/off recieived");
    if ( buffer[0] == '0' )
    {
      status = 0;
    } else {
      status = 1;
    }
    updateStatus(); 
      
  }

}

//--------------------------------------//

//--------------------------------------------------------------------------
void setup() {

  M5.begin();
  M5.Axp.ScreenBreath(0); // Set the initial backlight brightness to 'off'
  Serial.begin(115200);

  M5.Lcd.fillScreen(BLACK);

// establishing connection
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  #ifdef USE_EAP
    esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_ANONYMOUS_IDENTITY, strlen(EAP_ANONYMOUS_IDENTITY));
    esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
    esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
    esp_wifi_sta_wpa2_ent_enable();
    WiFi.begin(ssid); //connect to wifi
  //#else
    //WiFi.begin(ssid,WPA_PASSWORD);
  #endif
    WiFi.setSleep(false);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Waiting for connection");
    M5.Lcd.print("Waiting for connection");
  }
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
  M5.Lcd.print(ip);

//------- CONNECTING & SUBSCRIBING -------//
//added after connected in setup
  mqttClient.onMessage(onMqttMessage);
  mqttClient.setUsernamePassword("giiuser", "giipassword");
  mqttClient.connect(broker, port);
  mqttClient.subscribe(light_control_color);
  mqttClient.subscribe(light_control_status);
  //mqttClient.subscribe(light_status);
//---------------------------------------//

// Set up button
pinMode(button, INPUT_PULLUP);

M5.Lcd.fillScreen(rgb565(r,g,b));

}
//--------------------------------------------------------------------------
int waitingForRelease = 0; 

void loop(){

//------- USED TO TEST CONNECTION -------//
  //mqttClient.poll();
  //mqttClient.beginMessage(endpoint);
  //mqttClient.print("Hello From M5");
  //mqttClient.endMessage();
//--------------------------------------//


  mqttClient.poll();

  unsigned long last_time = millis();

  if(millis() - last_time > 2000)
  {
    sendStatus();
    last_time = millis();
  }  

  if(digitalRead(button)==LOW && !waitingForRelease)
  {
    status = !status;
    updateStatus();
    sendStatus();
    waitingForRelease = 1;   

  }  

  if(digitalRead(button)==HIGH)
  {
    waitingForRelease = 0;
  }
}
//--------------------------------------------------------------------------
