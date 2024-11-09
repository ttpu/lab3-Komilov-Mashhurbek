#include <WiFi.h>
#include <PubSubClient.h>

#define BTN 33

#define red 4
#define green 5
#define yellow 19
#define blue 22

const char* ssid = "Redmi Note 12 Pro";
const char* pass = "xzhl8363";

WiFiClient wifi_client;
PubSubClient mqtt_client(wifi_client);

void setup() {
  // configure button as input
  pinMode(BTN, INPUT);
  // configure serial port speed
  Serial.begin(115200);

  // LED configurations
  pinMode(red,OUTPUT);
  pinMode(green,OUTPUT);
  pinMode(yellow,OUTPUT);
  pinMode(blue,OUTPUT);

  WiFi.mode(WIFI_STA);
  delay(100);
  WiFi.begin(ssid, pass);
  delay(100);

  while (WiFi.status() != WL_CONNECTED){
    Serial.println("trying to connect....");
    delay(1000);
  }
  Serial.println("WiFi Connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());

  mqtt_client.setServer("mqtt.iotserver.uz", 1883);
  mqtt_client.setCallback(my_callback);
  
}

void loop() {

  while(!mqtt_client.connected()){

    // generate unique id
    
    String macAddr = WiFi.macAddress();
    String macSuffix = macAddr.substring(macAddr.length() - 5);
    macSuffix.replace(":", "");
    
    String myID = "edacs_lab4";
    myID += macSuffix;
    
    // connect to mqtt broker
    int mres = mqtt_client.connect(myID.c_str(), "userTTPU", "mqttpass");
    if (mres){
      Serial.print("mqtt connected: ");
      Serial.println(mres);
      mqtt_client.subscribe("ttpu/edacs/lab4");
      mqtt_client.publish("ttpu/edacs/msg", "hello");
    }
    else{
      Serial.print("mqtt NOT connected, trying: ");
      Serial.println(mres);
      delay(3000);
    }
  }

  mqtt_client.loop();
  
  // this variable keeps the counting number of button pressed event
  static int count = 0;
  // this variable keeps its value even the function is called multiple times
  static int prev_btn = LOW;
  // read current button state (HIGH means pressed)
  int btn = digitalRead(BTN);

  // detect button pressed event
  if (btn == HIGH && prev_btn == LOW){
    count++;
    Serial.println(count);
    
    digitalWrite(red,LOW);
    digitalWrite(green,LOW);
    digitalWrite(yellow,LOW);
    digitalWrite(blue,LOW);
    
    if (count == 1){
      digitalWrite(red,HIGH);  
    }
    else if (count == 2){
      digitalWrite(green,HIGH);
    }
    else if (count == 3){
      digitalWrite(yellow,HIGH);
    }
    else if (count == 4){
      digitalWrite(blue,HIGH);
    }

    if(count >= 5){
      count = 0;
    }
    
  }
  // save current btn state into a variable to be used in next loop iteration
  prev_btn = btn;

  delay(100);
}


void my_callback(char* topic, byte* payload, unsigned int len)
{
  Serial.println("Msg received");
}
