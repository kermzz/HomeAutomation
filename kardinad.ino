#include <Servo.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
int servoPin1 = D0;
int servoPin2 = D1;
String state;
char* control = "0";
Servo servo1;
Servo servo2;  
#define WLAN_SSID "RKKJ"
#define WLAN_PASS "pikadpasswordidpididheadolema"
#define mqtt_server "192.168.1.8"
#define mqtt_user "kardinad"
#define mqtt_password "kardinad"
char* kardinad = "kodu/kermz/kardinad";
WiFiClient wifiClient;
void callback(char* topic, byte* payload, unsigned int length);
PubSubClient client(mqtt_server, 1883, callback, wifiClient);

void setup() { 
  // put your setup code here, to run once:
  Serial.println("STARTING");
  Serial.begin(115200);
  delay(100);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  reconnect();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
void reconnect() {
 
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Connecting to ");
    Serial.println(WLAN_SSID);
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }
    Serial.println();
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
  
  if(WiFi.status() == WL_CONNECTED){
  // Loop until we're reconnected to the MQTT server
    while (!client.connected()) {
      Serial.print("Attempting MQTT connection...");

      // Generate client name based on MAC address and last 8 bits of microsecond counter
      String clientName;
      clientName += "esp8266-";
      uint8_t mac[6];
      WiFi.macAddress(mac);
      clientName += macToStr(mac);
      Serial.print(clientName);
      //if connected, subscribe to the topic(s) we want to be notified about
      if (client.connect((char*) clientName.c_str())) {
        Serial.print("\tMQTT Connected");
        client.subscribe(kardinad);
        client.subscribe("kodu/kermz/kardinad/currentstate");
      }

      //otherwise print failed for debugging
      else{Serial.println("\tFailed."); abort();}
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  //reconnect if connection is lost
  if (!client.connected() && WiFi.status() == 3) {reconnect();}

  //maintain MQTT connection
  client.loop();

  //MUST delay to allow ESP8266 WIFI functions to run
  delay(10); 
  /*servo.attach(servoPin);
  servo.write(60);
  delay(800);
  servo.write(0);
  delay(800);
  servo.detach();*/
}

String macToStr(const uint8_t* mac){

  String result;

  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);

    if (i < 5){
      result += ':';
    }
  }

  return result;
}
void callback(char* topic, byte* payload, unsigned int length) {

  //convert topic to string to make it easier to work with
  String topicStr = topic; 
  state = "";
  //Print out some debugging info
  Serial.println("Callback update.");
  Serial.print("Topic: ");
  Serial.println(topicStr);
  for(int i = 0; i < length; i++){
    state += (char)payload[i];
  }
  Serial.print(state);
    if(state == "Give current state")
    {      
      client.publish("kodu/kermz/kardinad/currentstate", control);
    }
 
 
  //turn the light on if the payload is '1' and publish to the MQTT server a confirmation message
  if(state == "kardin lahti"){
    if(control == "0"){
      client.publish("kodu/kermz/kardinad/state", "Avan kardinaid");
      servo1.attach(servoPin1);
      servo2.attach(servoPin2);
      servo1.write(0);
      servo2.write(0);
      delay(1000);
      servo1.detach();
      servo2.detach();
      control = "1";
      client.publish("kodu/kermz/kardinad/state", "Kardinad on lahti");
    }
    else 
    {
      return;
    }
  }

  //turn the light off if the payload is '0' and publish to the MQTT server a confirmation message
  else if (state == "kardin kinni"){
    if(control == "1")
    {
      client.publish("kodu/kermz/kardinad/state", "Sulgen kardinaid");
      servo1.attach(servoPin1);
      servo2.attach(servoPin2);
      servo1.write(180);
      servo2.write(180);
      delay(1000);
      servo1.detach();
      servo2.detach();
      control = "0";
      client.publish("kodu/kermz/kardinad/state", "Kardinad on kinni");
    }
    else 
    {
      return;
    }
  }

}
