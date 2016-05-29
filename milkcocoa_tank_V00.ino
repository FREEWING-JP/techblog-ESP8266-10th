git//tank control with milkcocoa
//2016 May 29th
//Cerevo Inc.

#include <ESP8266WiFi.h>
#include <Milkcocoa.h>

#define command_start  0
#define command_stop   1
#define command_back  2

#define AIN1 14
#define AIN2 13
#define BIN1 4
#define BIN2 5
#define PWM 1000
#define r_wait 1000


unsigned char drive_mode = 0;


char state = command_stop;


/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "********" //自宅で使用のSSID名
#define WLAN_PASS       "********"       //自宅で使用のSSID パスワード


/************************* Your Milkcocoa Setup *********************************/


#define MILKCOCOA_APP_ID      "*********"               //milkcocoa idを記載
#define MILKCOCOA_DATASTORE   "*********"            //milkcocoa data store 名を記載

/************* Milkcocoa Setup (you don't need to change this!) ******************/

#define MILKCOCOA_SERVERPORT  1883

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

const char MQTT_SERVER[] PROGMEM    = MILKCOCOA_APP_ID ".mlkcca.com";
const char MQTT_CLIENTID[] PROGMEM  = __TIME__ MILKCOCOA_APP_ID;

Milkcocoa milkcocoa = Milkcocoa(&client, MQTT_SERVER, MILKCOCOA_SERVERPORT, MILKCOCOA_APP_ID, MQTT_CLIENTID);

void onpush(DataElement *elem) {
  int command;    //1:f_left 2:forward 3:f_right 0:stop 7:r_left 8:back 9:r_right 4:left 5:right
  
  Serial.println("onpush");
  command = elem->getInt("com");

  if(command == 1){
    handle_f_left();
  }else if(command == 2){
    handle_drive();
  }else if(command == 3){
    handle_f_right();
  }else if(command == 0){
    handle_stop();
  }else if(command == 7){
    handle_r_left();
  }else if(command == 8){
    handle_back();
  }else if(command == 9){
    handle_r_right();
  }else if(command == 4){
    handle_left();
  }else if(command == 5){
    handle_right();
  }
 
  
};

void setupWiFi() {
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println("Milkcocoa SDK demo");

  setupWiFi();

    
  Serial.println( milkcocoa.on(MILKCOCOA_DATASTORE, "push", onpush) );
};


//Motor Driver DRV8835
void DRV8835_Control(){
    if (drive_mode == 0){
      analogWrite(AIN1,1024);
      analogWrite(AIN2,1024);
      analogWrite(BIN1,1024);
      analogWrite(BIN2,1024);
    }
    if (drive_mode == 1){
      analogWrite(AIN1,PWM);
      analogWrite(AIN2,0);
      analogWrite(BIN1,PWM);
      analogWrite(BIN2,0);
    }
    if (drive_mode == 2){
      analogWrite(AIN1,0);
      analogWrite(AIN2,PWM);
      analogWrite(BIN1,0);
      analogWrite(BIN2,PWM);
    }
    if (drive_mode == 3){
      analogWrite(AIN1,0);
      analogWrite(AIN2,PWM);
      analogWrite(BIN1,PWM);
      analogWrite(BIN2,0);
    }
    if (drive_mode == 4){
      analogWrite(AIN1,PWM);
      analogWrite(AIN2,0);
      analogWrite(BIN1,0);
      analogWrite(BIN2,PWM);
    }
    if (drive_mode == 5){
      analogWrite(AIN1,0);
      analogWrite(AIN2,0);
      analogWrite(BIN1,PWM);
      analogWrite(BIN2,0);
    }
    if (drive_mode == 6){
      analogWrite(AIN1,PWM);
      analogWrite(AIN2,0);
      analogWrite(BIN1,0);
      analogWrite(BIN2,0);
    }

}


void loop() {
  milkcocoa.loop();
  DataElement elem = DataElement();

  delay(1500);
};

void handle_stop() {
  Serial.print("stop\r\n");
  drive_mode = 0;
  DRV8835_Control();

    state = command_stop;
}

void handle_drive() {
 Serial.print("drive\r\n");
  drive();
  drive_mode = 1;  
  DRV8835_Control();

}

void handle_back() {
  Serial.print("back\r\n");
  drive();
  drive_mode = 2; 
  DRV8835_Control();

}

void handle_left(){
  Serial.print("left\r\n");
  drive();
  drive_mode = 3;  
  DRV8835_Control();
  
}

void handle_right(){
  Serial.print("right\r\n");
  drive_mode = 4;  
  DRV8835_Control();
  
}


void handle_f_left(){
  Serial.print("f_left\r\n");
  drive();
  drive_mode = 5;
  DRV8835_Control();
  delay(r_wait);  
  drive_mode = 1;
  DRV8835_Control();
  delay(r_wait);  
  
}


void handle_f_right(){
  Serial.print("f_right\r\n");
  drive_mode = 6;  
  DRV8835_Control();
  delay(r_wait);
  drive_mode = 1;  
  DRV8835_Control();
  delay(r_wait);  
}

void handle_r_left(){
  drive();
  Serial.print("r_left\r\n");
  drive_mode = 6;
  DRV8835_Control();
  delay(r_wait);
  drive_mode = 2;  
  DRV8835_Control();
  delay(r_wait);  
}


void handle_r_right(){
  Serial.print("r_right\r\n");
  drive();
  drive_mode = 5;
  DRV8835_Control();
  delay(r_wait);
  drive_mode = 2;  
  DRV8835_Control();
  delay(r_wait);  
}

void drive(){
    if(state == command_back){
    drive_mode = 2;
    DRV8835_Control();

     delay(10);

  
  }else if(state == command_stop){
    drive_mode = 0;
    DRV8835_Control();
  }
  state = command_start;
}


void back(){
    if(state == command_start){
    drive_mode = 1;
    DRV8835_Control(); 

     delay(10);

  
  }else if(state == command_stop){
    drive_mode = 0;
    DRV8835_Control();

  }
  state = command_back;
}
