#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// WIFI:
#define CONFIG_WIFI_SSID "KettleNet"
#define CONFIG_WIFI_PASSWORD "z7~8ys,h8l#hg'asu6g)`"

// MQTT:
const char* mqtt_server = "192.168.5.1";
DynamicJsonDocument settings_json(1024);

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg_5s = 0; // 5s delay
long lastMsg_1s = 0; // 1s delay
long lastMsg_100ms = 0; // 0.1s delay
//char msg[50];
int value = 0;

// Measured values that can be bassed between processes
float preferred_temp = 0; // in C
float temp_reading = 0; // in C
float standby_temp = 0; // idle standby temperature
float standby_temp_deviation = 0; // over/undershoot
float mass_level = 0; // in kg
int enable_standby = 0; // enable the standby process
int relay_set_state = 0; // bin 1/0
int btn_press_req = 0; // signal for the interrupt to switch modes.

// Safety Limits:
int abs_max_temp = 130; // [C] hard coded maximum temperature for power cutoff
float min_mass = 0.020; // minimum mass that can be in the kettle for relay to close

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
int hcnt = 0;

// Pins:
#define RELAY_PIN 25
#define THERMISTER_PIN 35
#define WHEATSTONE_PIN 39
#define PWR_BTN 27

// Function definitions:
void setup_wifi(void);
void callback(char* topic, byte* message, unsigned int length);
void reconnect(void);
void btn_ISR(void);
void isr_switcher(void);

void instrumentation(void);
void control(void);
void hid_display(void);
void report_status(void);

//enum Mode { IDLE, BUTTON_PRESS, HEAT_TO_PREF, HEAT_TO_STANDBY };
enum Mode { IDLE, HEAT_TO_PREF, HEAT_TO_STANDBY, OT_ERROR };
Mode KettleMode;
Mode PreviousKettleMode;

void setup() {
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
    Serial.begin(115200);

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
    }

    display.clearDisplay();// Clear the buffer
    display.setCursor(0,0);
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.println(F("Starting"));
    display.println(F("Kettle OS"));
    display.display();
    delay(2000);

    // WiFi Netowrk:
    setup_wifi();


    // MQTT
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);

    KettleMode = IDLE; // initial state
    // PreviousKettleMode = KettleMode;

    // Show initial display buffer contents on the screen --
    // the library initializes this with an Adafruit splash screen.
    // display.display();
    // delay(1000); // Pause for 2 seconds


    // GPIO
    pinMode(RELAY_PIN,OUTPUT);
    pinMode(16,OUTPUT);
    pinMode(PWR_BTN,INPUT);
    attachInterrupt(PWR_BTN, &btn_ISR, FALLING);
    digitalWrite(RELAY_PIN, LOW);
}

long now; // 5s loop iterator
long last_ctrl_iter = 0;
void loop() {
    // put your main code here, to run repeatedly:
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    now = millis();
    if (now - lastMsg_5s > 5000) {
        lastMsg_5s = now;
        report_status();
        // client.publish("kettle_settings","?"); // request current settings
    }

    // if (now - lastMsg_1s > 1000) {
    //     lastMsg_1s = now;
    //     // hid_display();
    //     // display.clearDisplay();
    //     // display.display();
    //     // report_status();
    // }

    if (now - lastMsg_100ms > 100) {
        lastMsg_100ms = now;
        isr_switcher();
        instrumentation();
        //control();
    }

    if( now - last_ctrl_iter > 500){
        hid_display();
        control();
    }

    // indicator LED:
    digitalWrite(16,digitalRead(RELAY_PIN));
}

void btn_ISR(void){
    //digitalWrite(RELAY_PIN, !digitalRead(RELAY_PIN));
    Serial.println("Button Press Interrupt");
    //heat_request
    //PreviousKettleMode = KettleMode;
    //KettleMode = BUTTON_PRESS;
    btn_press_req = 1;
}

void isr_switcher(void){
    if(btn_press_req == 1){
        if(KettleMode == IDLE || KettleMode == HEAT_TO_STANDBY){
            client.publish("kettle_settings_req","?");
            //PreviousKettleMode = KettleMode;
            KettleMode = HEAT_TO_PREF;
        }
        else if(KettleMode == HEAT_TO_PREF){
            //PreviousKettleMode = KettleMode;
            KettleMode = IDLE;
        }
        btn_press_req = 0;
    }
}

// =============================================================================
// Modules:
void setup_wifi(){
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(CONFIG_WIFI_SSID);

    // Show on the display
    display.clearDisplay();// Clear the buffer
    display.setCursor(0,0);
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.println(F("WiFi Net:"));
    display.println(F(CONFIG_WIFI_SSID));
    display.display();
    //delay(2000);

    WiFi.begin(CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);
    WiFi.setTxPower(WIFI_POWER_2dBm);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print("*");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

// .............................................................................
// Instrumentation Functions
int raw_v;
float value_i;

#define ARRLEN 20
float reading_array[ARRLEN] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int reading_index = 0;

float thermal_read(void){
    float value = 0;
    raw_v = analogRead(THERMISTER_PIN);
    // float value = raw_v*(5/4096)*(4.7/6.9);
    //float value = raw_v*.000831493;
    value_i = raw_v*-0.0295+135.29;

    reading_array[reading_index++] = value_i;
    if(reading_index == ARRLEN) reading_index = 0;


    for(int k=0; k < ARRLEN; k++)
        value = value + reading_array[k]/ARRLEN;

    return value;
}

// float mass_read(){
//     int raw_v = analogRead(WHEATSTONE_PIN);
//
//     return (1/2)*raw_v;
// }

// This function wraps up all the instrumentation processes and WRITES the
// values to the shared variables
void instrumentation(void){
    temp_reading = thermal_read();

}

// Display function for the 128x32px OLED
int hid_iter = 0;
int idle_iter = 0;

void hid_display(){
    display.clearDisplay();
    display.setCursor(0,0);             // Start at top-left corner
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    // display.println(F("Hello, world!"));
    //display.println(F("Hello, world!"));
    // display.println((char)int(temp_reading*1.8+32)+" / 175F");
    // display.print(int(temp_reading*1.8+32));

    if(KettleMode == HEAT_TO_PREF || KettleMode == HEAT_TO_STANDBY){
        switch (hcnt) {
            case 0: display.print(F("Heating   ")); break;
            case 1: display.print(F("Heating.  ")); break;
            case 2: display.print(F("Heating.. ")); break;
            case 3: display.print(F("Heating...")); break;
        }
        if(hid_iter++ == 15){ // slow down the heating update
            hcnt++;
            if(hcnt == 4) hcnt = 0;
            hid_iter = 0;
        }


        display.println("");
        display.print((int) (temp_reading*1.8+32));
        display.print("/");
        if(KettleMode == HEAT_TO_PREF){
            display.print((int) (preferred_temp*1.8+32));
        }
        else{
            display.print((int) (standby_temp*1.8+32));
        }
        //display.display();
        idle_iter = 0;
    }
    if(KettleMode == IDLE){
        if (idle_iter++ < 1535){
            display.println(F("Idle"));
            display.println((int) (temp_reading*1.8+32));
        }
    }
    if(KettleMode == OT_ERROR){
        display.println(F("OT ERROR"));
        display.print((int) temp_reading*1.8+32);
        // display.println(F("C"));
        display.print(F(">T_max"));

    }
    display.display();
}

// -----------------------------------------------------------------------------
// Control of stuff

void idle_state(void){
    //Serial.println("IDLE State");
    if ( (temp_reading < (standby_temp-standby_temp_deviation)) & (enable_standby==1) ){
        //PreviousKettleMode = KettleMode;
        KettleMode = HEAT_TO_STANDBY;
    }
    digitalWrite(RELAY_PIN, LOW);
}

void heat_to_standby(){
    //Serial.println("HEAT_TO_STANDBY State");
    //Serial.print("heating to:");
    //Serial.println(standby_temp);
    if( temp_reading < (standby_temp+standby_temp_deviation) )
        digitalWrite(RELAY_PIN, HIGH);
    else{
        digitalWrite(RELAY_PIN, LOW);
        //PreviousKettleMode = KettleMode;
        KettleMode = IDLE;
    }
}

void heat_to_pref(){
    // Serial.println("HEAT_TO_PREF State");
    // Serial.print("heating to:");
    // Serial.println(preferred_temp);
    if( temp_reading < preferred_temp )
        digitalWrite(RELAY_PIN, HIGH);
    else{
        digitalWrite(RELAY_PIN, LOW);
        //PreviousKettleMode = KettleMode;
        KettleMode = IDLE;
    }
}

// void button_press(){
//     Serial.println("Button Press State");
//     if(PreviousKettleMode == IDLE || PreviousKettleMode == HEAT_TO_STANDBY){
//         client.publish("kettle_settings_req","?");
//         PreviousKettleMode = KettleMode;
//         KettleMode = HEAT_TO_PREF;
//     }
//     else if(PreviousKettleMode == HEAT_TO_PREF){
//         PreviousKettleMode = KettleMode;
//         KettleMode = IDLE;
//     }
// }

void control(){

    // Safety Check:
    if(((int)temp_reading) > abs_max_temp)
        KettleMode = OT_ERROR;
    // Control routines
    switch (KettleMode){
        case IDLE: idle_state(); break;
        //case BUTTON_PRESS: button_press(); break;
        case HEAT_TO_PREF: heat_to_pref(); break;
        case HEAT_TO_STANDBY: heat_to_standby(); break;
        case OT_ERROR: digitalWrite(RELAY_PIN, LOW) ;break;
        //default: KettleMode = IDLE; break;
    }

}


// Status Reporting
void report_status(void){
    char msg_buf[64+32];
    int water_setpint;

    switch (KettleMode){
        case IDLE: water_setpint=0; break;
        case HEAT_TO_PREF: water_setpint=(int) preferred_temp; break;
        case HEAT_TO_STANDBY: water_setpint=(int) standby_temp; break;
        case OT_ERROR: water_setpint=-1; break;
        default: water_setpint=0; break;
    }

    sprintf(msg_buf,"{\"mode\":%i,\"cur_temp\":%d,\"set_temp\":%d,\"RawADC\":%d}",KettleMode,(int) temp_reading,water_setpint,raw_v);
    client.publish("kettle_status",msg_buf);
}

// -----------------------------------------------------------------------------
// MQTT Stuff
void callback(char* topic, byte* message, unsigned int length) {
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");
    String messageTemp;

    for (int i = 0; i < length; i++) {
        Serial.print((char)message[i]);
        messageTemp += (char)message[i];
    }
    Serial.println();

    // Feel free to add more if statements to control more GPIOs with MQTT

    // If a message is received on the topic esp32/output, you check if the message is either "on" or "off".
    // Changes the output state according to the message
    if (String(topic) == "kettle_command") {
        //Serial.print("Changing output to ");
        if(messageTemp == "true"){
            //Serial.println("LED0 ON");
            //digitalWrite(RELAY_PIN, HIGH);
            //PreviousKettleMode = KettleMode;
            //KettleMode = BUTTON_PRESS;
            KettleMode = HEAT_TO_PREF;
        }
        else if(messageTemp == "false"){
            //Serial.println("LED0 OFF");
            //digitalWrite(RELAY_PIN, LOW);
            //PreviousKettleMode = KettleMode;
            //KettleMode = BUTTON_PRESS;
            KettleMode = IDLE;
        }
        // btn_press_req = 1;
    }


    // Kettle Settings
    if (String(topic) == "kettle_settings") {
        Serial.println("Kettle Settings Payload!");
        char mtc[length];
        for (int k = 0; k < length; k++) {
            // Serial.print((char)message[k]);
            mtc[k] = message[k];
        }
        // Convert to JSON
        DeserializationError error = deserializeJson(settings_json, mtc);
        if(error){
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
        }

        //Serial.print("preferred_temp:");
        preferred_temp = (float) settings_json["preferred_temp"];
        //Serial.println((int)settings_json["preferred_temp"]);

        //Serial.print("standby_temp:");
        standby_temp = (float) settings_json["standby_temp"];
        //Serial.println((int)settings_json["standby_temp"]);

        //Serial.print("standby_temp_deviation:");
        standby_temp_deviation = (float) settings_json["standby_temp_deviation"];
        //Serial.println((int)settings_json["standby_temp_deviation"]);

        //Serial.print("enable_standby:");
        enable_standby = (int) settings_json["enable_standby"];
        //Serial.println((int)settings_json["enable_standby"]);
        // Serial.println((char*)doc["standby_temp"]);
    }
}

int broker_reconnect_attempt = 0;
void reconnect() {
    // Loop until we're reconnected
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect("KETTLE_IED","DerpMcDerpertopn","AXQUCyaLdcVZiJA7SSZqC3ugCz")) {
            Serial.println("connected");
            // Subscribe
            client.subscribe("kettle_command");
            client.subscribe("kettle_settings");
            client.subscribe("kettle_settings_req");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);

            Serial.print("Broker reconnect attempt: ");
            Serial.println(broker_reconnect_attempt);
            if(broker_reconnect_attempt >= 5 && WiFi.status() != WL_CONNECTED){
                broker_reconnect_attempt = 0;
                Serial.println("Resetting to WiFi");
                WiFi.disconnect();
                delay(1000);
                setup_wifi();
            }
            broker_reconnect_attempt+=1;
        }
    }
}
