//  CPU ESP-WROON-02  etc
//  SENSOR BME280
//  LCD SC1602 SC2004 SSD1306 SC1601
//  Ambient IoT
//「ESP8266 core for Arduino」をバージョン2.7未満


//I2Cのポートを選ぶ
#define bme_vcc 14

//sda,scl  wemos d1r2  A5,A4
//#define i2c_sda 4
//#define i2c_scl 5

//sda,scl  esp-01
#define i2c_sda 2
#define i2c_scl 0

//wemos d1r2  mosi_d7,miso_d6
//#define i2c_sda 12
//#define i2c_scl 13


//include
#include <Wire.h>
#include <ESP8266WiFi.h>  
#include <Ambient.h>
#include <time.h>
#include <LiquidCrystal_I2C.h>
// PCF8574のアドレスと、LCDのサイズを選ぶ
//  LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
  LiquidCrystal_I2C lcd(0x27,20,4);  
//  LiquidCrystal_I2C lcd(0x3f,16,2);
//  LiquidCrystal_I2C lcd(0x3f,20,4);  

#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
  ESP8266WebServer Server(80);         //  ウェブサーバ＠ポート番号（HTTP）
  ESP8266HTTPUpdateServer Updater;     //  ウェブアップデート
#include <Ticker.h>
  Ticker ticker;


#include <BME280_MOD-1022.h>




//ADCはVCC電圧測定モード
ADC_MODE(ADC_VCC)

// ESP8266が接続するWi-Fiアクセスポイントの設定
const char* ssid1 = "AAAAA";
const char* password1 = "aaaaa";
const char* ssid2 = "BBBBB";
const char* password2 = "bbbbb";
const char* ssid3 = "CCCCC";
const char* password3 = "ccccc";
const char* ssid4 = "DDDDD";
const char* password4 = "ddddd";
const char* ssid5 = "EEEEE";
const char* password5 = "eeeee";
const char* ssid6 = "FFFFF";
const char* password6 = "fffff";

// Ambientのチャネル設定
unsigned int channelId =  ;
const char* writeKey = "";


WiFiClient client;
Ambient ambient;



// WiFi Setting
#define JST     3600*9

//割り込みに伴うグローバル化
bool  ambient_flg;
bool  flg_1s = true ; 
struct tm *tm;
static const char *wd[7] = {"Sun","Mon","Tue","Wed","Thr","Fri","Sat"};
time_t t;
float temperature;  
float humidity ;
float pressure ;
bool dot_flg = true ;

const char lcd_dot[6] = {0x00,0x10,0x18,0x1c,0x1e,0x1f};

//const char lcd_cg[][8] = {  
uint8_t lcd_cg[][8] = {  
    { //0x00
      0b00000,
      0b00000,
      0b00000,
      0b00000,
      0b00000,
      0b00000,
      0b00000,
      0b00000
    },
    { //0x01 黒
      0b11111,
      0b11111,
      0b11111,
      0b11111,
      0b11111,
      0b11111,
      0b11111,
      0b11111
    },
    { //0x02
      0b00000,
      0b00000,
      0b11111,
      0b11111,
      0b11111,
      0b11111,
      0b11111,
      0b11111
    },

    { //0xFF
      0b11111,
      0b11111,
      0b11111,
      0b11111,
      0b11111,
      0b11111,
      0b00000,
      0b00000
    },

    { //0x04
      0b11111,
      0b11111,
      0b00000,
      0b00000,
      0b00000,
      0b00000,
      0b11111,
      0b11111
    },
    { //0x05
      0b00000,
      0b00000,
      0b00000,
      0b00000,
      0b11111,
      0b11111,
      0b11111,
      0b11111
    },
    { //0x06
      0b00000,
      0b00000,
      0b11111,
      0b11111,
      0b11111,
      0b11111,
      0b00000,
      0b00000
    },
    { //0x07
      0b11111,
      0b11111,
      0b11111,
      0b11111,
      0b00000,
      0b00000,
      0b00000,
      0b00000
    }
};


//const char lcd_cg_second[][8] = {  
uint8_t lcd_cg_second[][8] = {  

    { //0x00
      0b00000,
      0b00000,
      0b00000,
      0b00000,
      0b00000,
      0b00000,
      0b00000,
      0b00000      
    },
    { //0x10
      0b10000,
      0b10000,
      0b10000,
      0b10000,
      0b00000,
      0b00000,
      0b00000,
      0b00000

    },
    { //0x18
      0b11000,
      0b11000,
      0b11000,
      0b11000,
      0b00000,
      0b00000,
      0b00000,
      0b00000
    },
    { //0x1C
      0b11100,
      0b11100,
      0b11100,
      0b11100,
      0b00000,
      0b00000,
      0b00000,
      0b00000
    },
    { //0x1E
      0b11110,
      0b11110,
      0b11110,
      0b11110,
      0b00000,
      0b00000,
      0b00000,
      0b00000
    },
    { //0x1F
      0b11111,
      0b11111,
      0b11111,
      0b11111,
      0b00000,
      0b00000,
      0b00000,
      0b00000
    },

    { //0x110
      0b11111,
      0b11111,
      0b11111,
      0b11111,
      0b10000,
      0b10000,
      0b10000,
      0b10000
    },
    { //0x118
      0b11111,
      0b11111,
      0b11111,
      0b11111,
      0b11000,
      0b11000,
      0b11000,
      0b11000
    },
    { //0x11C
      0b11111,
      0b11111,
      0b11111,
      0b11111,
      0b11100,
      0b11100,
      0b11100,
      0b11100
    },
    { //0x11E
      0b11111,
      0b11111,
      0b11111,
      0b11111,
      0b11110,
      0b11110,
      0b11110,
      0b11110
    },

    { //0x11F
      0b11111,
      0b11111,
      0b11111,
      0b11111,
      0b11111,
      0b11111,
      0b11111,
      0b11111
    }
};


const char LCD_FONT[][12] = {
    { //0
      0x05,0x07,0x07,0x05,
      0xFF,0x20,0x20,0xFF,
      0x07,0x05,0x05,0x07
    },
    { //1
      0x20,0x05,0xFF,0x20,
      0x20,0x20,0xFF,0x20,
      0x20,0x05,0xFF,0x05
    },
    { //2
      0x05,0x07,0x07,0x05,
      0x05,0x06,0x06,0x07,
      0xFF,0x05,0x05,0x05
    },
    { //3
      0x06,0x07,0x07,0x05,
      0x20,0x06,0x06,0xFF,
      0x06,0x05,0x05,0x07
    },
    { //4
      0x20,0x05,0xFF,0x20,
      0xFF,0x05,0xFF,0x05,
      0x20,0x20,0xFF,0x20

    },
    { //5
      0xFF,0x07,0x07,0x07,
      0x07,0x06,0x06,0x05,
      0x06,0x05,0x05,0x07
    },
    { //6
      0x05,0x07,0x20,0x20,
      0xFF,0x06,0x06,0x05,
      0x07,0x05,0x05,0x07
    },
    { //7
      0xFF,0x07,0x07,0xFF,
      0x20,0x20,0x05,0x07,
      0x20,0x20,0xFF,0x20
    },
    { //8
      0x05,0x07,0x07,0x05,
      0xFF,0x06,0x06,0xFF,
      0x07,0x05,0x05,0x07
    },
    { //9
      0x05,0x07,0x07,0x05,
      0x07,0x06,0x06,0xFF,
      0x20,0x20,0x05,0x07
    }
};

uint8_t lcd_cg_Numbers[][8] = {  
    { //0
      0b01110,
      0b10001,
      0b10011,
      0b10101,
      0b11001,
      0b10001,
      0b01110,
      0b00000
    },
    { //1
      0b00100,
      0b01100,
      0b00100,
      0b00100,
      0b00100,
      0b00100,
      0b01110,
      0b00000
    },
    { //2
      0b01110,
      0b10001,
      0b00001,
      0b00010,
      0b00100,
      0b01000,
      0b11111,
      0b00000
    },
    { //3
      0b11111,
      0b00010,
      0b00100,
      0b00010,
      0b00001,
      0b10001,
      0b01110,
      0b00000
    },
    { //4
      0b00010,
      0b00110,
      0b01010,
      0b10010,
      0b11111,
      0b00010,
      0b00010,
      0b00000
    },
    { //5
      0b11111,
      0b10000,
      0b11110,
      0b00001,
      0b00001,
      0b10001,
      0b01110,
      0b00000
    },
    { //6
      0b00110,
      0b01000,
      0b10000,
      0b11110,
      0b10001,
      0b10001,
      0b01110,
      0b00000
    },
    { //7
      0b11111,
      0b00001,
      0b00010,
      0b00100,
      0b01000,
      0b01000,
      0b01000,
      0b00000
    },
    { //8
      0b01110,
      0b10001,
      0b10001,
      0b01110,
      0b10001,
      0b10001,
      0b01110,
      0b00000
    },
    { //9
      0b01110,
      0b10001,
      0b10001,
      0b01111,
      0b00001,
      0b00010,
      0b01100,
      0b00000
    },
    { //(スペース)
      0b00000,
      0b00000,
      0b00000,
      0b00000,
      0b00000,
      0b00000,
      0b00000,
      0b00000   
    },    
    { //:
      0b00000,
      0b01100,
      0b01100,
      0b00000,
      0b01100,
      0b01100,
      0b00000,
      0b00000   
    },
    { //%
      0b11000,
      0b11001,
      0b00010,
      0b00100,
      0b01000,
      0b10011,
      0b00011,
      0b00000   
    },
    { //°
      0b11100,
      0b10100,
      0b11100,
      0b00000,
      0b00000,
      0b00000,
      0b00000,
      0b00000   
    },
    { //℃
      0b11000,
      0b11000,
      0b00000,
      0b01111,
      0b01000,
      0b01000,
      0b01111,
      0b00000   
    }
  };


void setIntForTicker() {
  dot_flg = !dot_flg;
  if (dot_flg){
    ESP.wdtDisable();
    loop_serial();    

//    loop_lcdprint_SC1601_2();		//lcd sc1602用

    loop_lcdprint();			//lcd sc2004用

  }else{


    BME280.readCompensationParams();
    
    // Need to turn on 1x oversampling, default is os_skipped, which means it doesn't measure anything
    BME280.writeOversamplingPressure(os1x);  // 1x over sampling (ie, just one sample)
    BME280.writeOversamplingTemperature(os1x);
    BME280.writeOversamplingHumidity(os1x);

    // example of a forced sample.  After taking the measurement the chip goes back to sleep
    BME280.writeMode(smForced);

    lcd.setCursor(9, 0);
    lcd.print(' ');
    lcd.setCursor(9, 2);
    lcd.print(' '); 

    for (unsigned short escape_count=0;escape_count<2047;escape_count++) {
      if (!BME280.isMeasuring()){
        // BME280から測定値を読み取る
        BME280.readMeasurements();
        temperature = BME280.getTemperature();  // must get temp first
        humidity = BME280.getHumidity();
        pressure = BME280.getPressure();
        break;
      }
    }
  }

  
}


void setup() {
  wifi_set_sleep_type(LIGHT_SLEEP_T);     //スリープモードの選択
  WiFi.mode(WIFI_STA); 

  i2c_soft_reset();
  Wire.begin(i2c_sda,i2c_scl );         //sda,scl  
  Wire.setClock(100000L);               //rfc8574が100kbps
  pinMode(bme_vcc, OUTPUT);             //bme280 vcc
  digitalWrite(bme_vcc, HIGH);

  Serial.begin(115200);
  Serial.println("");
  
  uint8_t chipID = BME280.readChipId();
  Serial.print("BME280 Chip ID: 0x");
  Serial.println(chipID, HEX);

  Serial.println("ESP-WROOM-02 SC2004 BME280 START");
  Serial.print("channelId = ");
  Serial.println(channelId);
 

  // Print a message to the LCD.
   // set up the LCD's number of columns and rows:
  lcd.begin(i2c_sda,i2c_scl); //ESP8266-01 I2C with pin 0-SDA 2-SCL
  lcd.backlight();
  lcd.clear();
  lcd.print("BME280 SC2004 ESP-WROOM-02");  

  ESP.wdtDisable();
  wifi_connect();   

  //CGRAMにたまごめ
  for(unsigned int i=0; i< 8; i++){
    lcd.createChar(i,lcd_cg[i]);
  }
  
  configTime(0, 0, "time.google.com" ,  "ntp.nict.jp");//NTPの設定
  //  ウェブアップロードの仕込み
  Updater.setup(&Server);
  Server.begin();

  // 0.5 秒ごとに setIntForTicker() を呼び出す
  ticker.attach_ms(500, setIntForTicker);
}

void loop() {  
  //-------------------------
  // ステーションモードに設定
  ESP.wdtDisable();

  // ステーションモードに設定
  WiFi.mode(WIFI_STA);
  //WIFI に接続
  int ssid_no = wifi_connect();
  int rssi = WiFi.RSSI();
  configTime(JST, 0,  "ntp.nict.jp", "time.google.com");//NTPの設定    

  // VCCを読み取る
  float esp_vcc = ESP.getVcc() ;  //mVで転送

  ambient.begin(channelId, writeKey, &client);

  Serial.print("channelId = ");
  Serial.println(channelId);
  Serial.print("Temp=");
  Serial.println(temperature);
  Serial.print("Humidity=");
  Serial.println(humidity);
  Serial.print("Pressure=");
  Serial.println(pressure);
    
  ambient.set(1, temperature);
  ambient.set(2, ssid_no);
  ambient.set(3, esp_vcc);
  ambient.set(4, humidity);
  ambient.set(5, pressure);
  ambient.set(6, rssi);
  ambient_flg = ambient.send();
  ambient_flg = true  ;

  do{
    //  クライアントからの要求を処理する
    Server.handleClient();  
    ESP.wdtDisable();
    delay(1000 - millis()%1000);    //割と正確な1秒待ち
    t = time(NULL);
  }while ((t%300)!=0);  
}

void loop_serial(void){
  t = time(NULL);
  tm = localtime(&t);
  Serial.print("localtime:");
//  Serial.print(tm->tm_year + 1804);
  Serial.print(tm->tm_year + 1900);
  Serial.print('/');
  Serial.print(tm->tm_mon+1);
  Serial.print('/');
  Serial.print(tm->tm_mday);
  Serial.print(' ');
  Serial.print(wd[tm->tm_wday]);
  Serial.print(' ');  
  Serial.print(tm->tm_hour);
  Serial.print(":");
  Serial.print(tm->tm_min);
  Serial.print(":");
  Serial.print(tm->tm_sec); 

  Serial.print(" ");
  Serial.print(temperature);
  Serial.println("℃");

}

void loop_lcdprint(void){
  //SC2004
  struct tm *tm;
  
  lcd.setCursor(0,0);   //上で代用
  tm = localtime(&t);
  lcd_time(t,0);
  lcd.setCursor(0, 1);
  lcd_time(t,1);
  lcd.setCursor(0, 2);
  lcd_time(t,2);  
  lcd_time_4(t);
}

void lcd_time( unsigned long tm_data , int line){
  char buffer;
  char buffer2;
  int i ;
  struct tm *tm;
  tm = localtime(&t);
  unsigned char barline,barcha,chaline ;
  barline = (unsigned char)(tm_data % 60);
  barcha = barline / 5 ;
  chaline = barline % 5 ;

//  char lcd_cg_second2[8] = {
  uint8_t lcd_cg_second2[8] = {       
       //0x00
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000
  };

  if (barline < 31){
    for (unsigned char j=0; j< 6; j++){
      if (barcha < j){
        lcd_cg_second2[j+2] = 0x00;
      }else if (barcha == j){
        lcd_cg_second2[j+2] = lcd_dot[chaline];
      }else{
        lcd_cg_second2[j+2] = 0xff;
      }
    }    
  }else{
    for (unsigned char j=0; j< 6; j++){
      if (barcha-6 < j){
        lcd_cg_second2[j] = 0x00;
      }else if (barcha-6 == j){
        lcd_cg_second2[j] = lcd_dot[chaline];
      }else{
        lcd_cg_second2[j] = 0xff;
      }
    }    
  }


  buffer2 = (tm->tm_hour) ;    // 十時の数値を ASCII 文字にする
  if (buffer2 >12){
    buffer2 = buffer2 -12;        //12時間表示
  }

  buffer = buffer2/10 ;    // 十時の数値を ASCII 文字にする  

  for(unsigned int i=1; i< 4; i++){
    if (buffer == 0){
      lcd.print(" ");
    }else{
      lcd.print(LCD_FONT[buffer][line * 4 + i]);
    }
//  delayMicroseconds(5);
  }
  lcd.print(" ");

  buffer = buffer2 %10 ;    // 一時の数値を ASCII 文字にする
  for(unsigned int i=0; i< 4; i++){
    lcd.print(LCD_FONT[buffer][line * 4 + i]);
//  delayMicroseconds(5);
  }

  switch(line){
    case 0:
      lcd.print(" ");
       if (barline == 0){
        lcd.write(0x05);
      }else if (barline > 30){
        lcd.write(0x05);
      }else{
        lcd.write(8);           
      }
      lcd.print(" ");
    break;
    
    case 1:
      lcd.print("   ");
    break;
    default:
      lcd.print(" ");
      if (barline == 0){
        lcd.write(0x06);
      }else if (barline > 30){
        lcd.write(8);
      }else{
        lcd.write(0x06);
      }
      lcd.print(" ");
    break;
  }

  buffer = (tm->tm_min)/10 ;      // 十分の数値を ASCII 文字にする
  for(unsigned int i=0; i< 4; i++){
    lcd.print(LCD_FONT[buffer][line * 4 + i]);
  }
  lcd.print(" ");
  buffer = (tm->tm_min)%10 ;      // 一分の数値を ASCII 文字にする
  for(unsigned int i=0; i< 4; i++){
    lcd.print(LCD_FONT[buffer][line * 4 + i]);
  }
  lcd.createChar(0,lcd_cg_second2);
}


void lcd_time_4(unsigned long tm_data){
  struct tm *tm;
  char buffer;
  tm = localtime(&t);
  
  lcd.setCursor(0, 3);  //4行目表示

  lcd.print((int)temperature);
  lcd.write(0xdf);
  lcd.print("C");
  lcd.print(" ");
  lcd.print((int)humidity);
  lcd.print("%");
  lcd.print(" ");


  lcd.print("  ");

  
  buffer = (tm->tm_hour)/10 + '0'  ;      // 十時の数値を ASCII 文字にする
  if (buffer == '0'){
    lcd.print(" ");
  }else{
    lcd.print(buffer);
  }
  buffer = (tm->tm_hour)%10 + '0' ;      // 一時の数値を ASCII 文字にする
  lcd.print(buffer);
  lcd.print(":");
  buffer = (tm->tm_min)/10 + '0' ;      // 十分の数値を ASCII 文字にする
  lcd.print(buffer);
  buffer = (tm->tm_min)%10 + '0' ;      // 一分の数値を ASCII 文字にする
  lcd.print(buffer);
  lcd.print(":");
  buffer = (tm->tm_sec)/10 + '0';       // 十秒の数値を ASCII 文字にする
  lcd.print(buffer);
  buffer = (tm->tm_sec)%10 + '0' ;      // 一秒の数値を ASCII 文字にする
  lcd.print(buffer);
}

void loop_lcdprint_SC1601(void){
  //SC1601
  struct tm *tm;
  char buffer;
  tm = localtime(&t);
  
  lcd.clear();  //1行目表示
 
  buffer = (tm->tm_hour)/10 + '0'  ;      // 十時の数値を ASCII 文字にする
  if (buffer == '0'){
    lcd.print(" ");
  }else{
    lcd.print(buffer);
  }
  buffer = (tm->tm_hour)%10 + '0' ;      // 一時の数値を ASCII 文字にする
  lcd.print(buffer);
  lcd.print(":");
  buffer = (tm->tm_min)/10 + '0' ;      // 十分の数値を ASCII 文字にする
  lcd.print(buffer);
  buffer = (tm->tm_min)%10 + '0' ;      // 一分の数値を ASCII 文字にする
  lcd.print(buffer);
  lcd.print(":");
  buffer = (tm->tm_sec)/10 + '0';       // 十秒の数値を ASCII 文字にする
  lcd.print(buffer);
  buffer = (tm->tm_sec)%10 + '0' ;      // 一秒の数値を ASCII 文字にする
  lcd.print(buffer);

  lcd.print(" ");
  lcd.print((int)humidity);
  lcd.print("% ");
  lcd.print((int)temperature);
  lcd.write(0xdf);
  lcd.print("C");
  lcd.print(" ");
}



//----------------------------------------------------------
void deep_sleep_process(){
  //DEEP SLEEPモード突入命令
  Serial.println("DEEP SLEEP START!!");
  delay(100);

  //1:μ秒での復帰までのタイマー時間設定  2:復帰するきっかけの設定（モード設定）
  ESP.deepSleep(2945 * 100 * 1000 , WAKE_RF_DEFAULT);    //コネクト等に平均5.5秒かかる　5分毎
  delay(100);
    
  //deepsleepモード移行までのダミー命令
  delay(1000);

  //実際にはこの行は実行されない
  Serial.println("DEEP SLEEPing....");
}
//-----------------------------------------------------------
int wifi_connect() {
  int i, loop;
  int stat;
  static int ssid_no ;

    for (loop = 1; loop <= 6; loop++) {

      Serial.print("WiFi(");
      if (ambient_flg == false){
        ssid_no++;
        ambient_flg = true ;  
      }

      switch (ssid_no){
        default :
          WiFi.begin(ssid1, password1);
          ssid_no = 1 ;
        break;
        case 2 :
          WiFi.begin(ssid2, password2);
        break;
        case 3 :
          WiFi.begin(ssid3, password3);
        break;        
        case 4 :
          WiFi.begin(ssid4, password4);        
        break;
        case 5 :
          WiFi.begin(ssid5, password5);        
        break;
        case 6 :
          WiFi.begin(ssid6, password6);        
        break;
      }

      Serial.print(ssid_no);
      Serial.print(") Connecting.");
      
      for (i = 0; i < 50; i++) {
        ESP.wdtDisable();
        stat = WiFi.status();
        if (stat == WL_DISCONNECTED) {
          delay(500);
          Serial.print(".");
        } else if (stat == WL_NO_SSID_AVAIL || stat == WL_CONNECT_FAILED) {
          break;  // break to for(i)
        } else if (stat == WL_CONNECTED) {
          Serial.println("WiFi connected!");
          Serial.print("IP address: ");
          Serial.println(WiFi.localIP());
          break;  // break for(i)
        }
      }
      if (stat == WL_CONNECTED) break;  // break for(loop)
      if (loop != 6){
        Serial.println("WiFi Failed. Connect to another AP");
        ambient_flg = false ;    
      } else {
        Serial.println("WiFi Failed.");
        Serial.println("Wait for some time");
      }
      WiFi.disconnect();
    }
  return ssid_no;
}

//-----------------------------------------------------------

void i2c_soft_reset() {
//i2c ソフトリセット
  pinMode(i2c_scl, OUTPUT_OPEN_DRAIN);  //clk
  pinMode(i2c_sda, OUTPUT_OPEN_DRAIN);  //sda
  //startコンディションを2回
  for (int i = 0; i < 1; i++) {
    digitalWrite(i2c_sda, HIGH);
    delayMicroseconds(5);
    digitalWrite(i2c_scl, HIGH);
    delayMicroseconds(5);
    digitalWrite(i2c_sda, LOW);
    delayMicroseconds(5);
    digitalWrite(i2c_scl, LOW);
    delayMicroseconds(5);
  }
  //clk を14パルス
  digitalWrite(i2c_sda, HIGH);
  for (int i = 0; i < 9; i++) {
    digitalWrite(i2c_scl, HIGH);
    delayMicroseconds(5);
    digitalWrite(i2c_scl, LOW);
    delayMicroseconds(5);
  }
  //startコンディションを2回
  for (int i = 0; i < 1; i++) {
    digitalWrite(i2c_sda, HIGH);
    delayMicroseconds(5);
    digitalWrite(i2c_scl, HIGH);
    delayMicroseconds(5);
    digitalWrite(i2c_sda, LOW);
    delayMicroseconds(5);
    digitalWrite(i2c_scl, LOW);
    delayMicroseconds(5);
  }
}


void loop_lcdprint_SC1601_2(void){
  //SC1601_2
  struct tm *tm;
  tm = localtime(&t);

  unsigned short barline;
  char buffer , barcha , chaline;

  barline = (unsigned char)(t % 60);
  barcha = barline / 5 ;
  chaline = barline % 5 ;



  if (barline ==0){         //ちらつき防止
    lcd.setCursor(11, 0);  //11文字目表示
    lcd.print("% ");
    lcd.print((int)temperature);
    lcd.write(0xdf);
    lcd.print("C");
  }
  
  lcd.setCursor(0, 0);  //1行目表示
  buffer = (tm->tm_hour)/10 ;      // 十時の数値を ASCII 文字にする
  lcd_reversibl_cg(buffer , barcha , chaline , 0);
  buffer = (tm->tm_hour)%10 ;      // 一時の数値を ASCII 文字にする
  lcd_reversibl_cg(buffer , barcha , chaline , 1);
  buffer = 11;  //:
  lcd_reversibl_cg(buffer , barcha , chaline , 2);
  buffer = (tm->tm_min)/10 ;      // 十分の数値を ASCII 文字にする
  lcd_reversibl_cg(buffer , barcha , chaline , 3);
  buffer = (tm->tm_min)%10 ;      // 一分の数値を ASCII 文字にする
  lcd_reversibl_cg(buffer , barcha , chaline , 4);
  buffer = 11;  //:
  lcd_reversibl_cg(buffer , barcha , chaline , 5);

  buffer = (tm->tm_sec)/10;       // 十秒の数値を ASCII 文字にする
  lcd_reversibl_cg(buffer , barcha , chaline , 6);
  buffer = (tm->tm_sec)%10;       // 一秒の数値を ASCII 文字にする
  lcd_reversibl_cg(buffer , barcha , chaline , 7);
  buffer = 10;
  lcd_reversibl_cg(buffer , barcha , chaline , 8);
  buffer = (char)humidity /10; 
  lcd_reversibl_cg(buffer , barcha , chaline , 9);
  buffer = (char)humidity %10; 
  lcd_reversibl_cg(buffer , barcha , chaline , 10);  
  buffer = 12;
  lcd_reversibl_cg(buffer , barcha , chaline , 11);
  buffer = 10;
  lcd_reversibl_cg(buffer , barcha , chaline , 12);
  buffer = (char)temperature /10; 
  lcd_reversibl_cg(buffer , barcha , chaline , 13);
  buffer = (char)temperature %10; 
  lcd_reversibl_cg(buffer , barcha , chaline , 14);  
  buffer = 13;
  lcd_reversibl_cg(buffer , barcha , chaline , 15);

}

void lcd_reversibl_cg(char buffer_cg , char barcha_cg , char chaline_cg , char chrno_cg){
  //                  表示する数字        秒を5で割る        秒を5で割った余り     桁(時分秒)
  lcd.setCursor( chrno_cg, 0);
  if (barcha_cg == chrno_cg){                   //ドットの左端
    switch(chaline_cg){
      case 5:
        lcd_cg_Numbers[buffer_cg][7] =  0x00;
      break;
      case 4:
        lcd_cg_Numbers[buffer_cg][7] =  0x01;
      break;
      case 3:
        lcd_cg_Numbers[buffer_cg][7] =  0x03;
      break;
      case 2:
        lcd_cg_Numbers[buffer_cg][7] =  0x07;
      break;
      case 1:
        lcd_cg_Numbers[buffer_cg][7] =  0x0f;
      break;
      default:
        lcd_cg_Numbers[buffer_cg][7] =  0x1f;

      break;
    }

    lcd.createChar( 0 , lcd_cg_Numbers[buffer_cg] );
    lcd.setCursor( chrno_cg, 0);
    lcd.write(0x00);    

  }else if ((barcha_cg + 4 ) == chrno_cg ){        //ドットの右端

    switch(chaline_cg){
      case 5:
        lcd_cg_Numbers[buffer_cg][7] =  0x1f;
      break;
      case 4:
        lcd_cg_Numbers[buffer_cg][7] =  0x1e;
      break;
      case 3:
        lcd_cg_Numbers[buffer_cg][7] =  0x1c;
      break;
      case 2:
        lcd_cg_Numbers[buffer_cg][7] =  0x18;
      break;
      case 1:
        lcd_cg_Numbers[buffer_cg][7] =  0x10;
      break;
      default:
        lcd_cg_Numbers[buffer_cg][7] =  0x00;
      break;
    }
    lcd.createChar( 4 , lcd_cg_Numbers[buffer_cg] );
    lcd.setCursor( chrno_cg, 0);
    lcd.write(0x04);

  }else if ((barcha_cg < chrno_cg )&( chrno_cg < (barcha_cg + 4))){
    lcd_cg_Numbers[buffer_cg][7] =  0x1f;
    barcha_cg = chrno_cg - barcha_cg ;
    lcd.createChar( barcha_cg , lcd_cg_Numbers[buffer_cg] );
    lcd.setCursor( chrno_cg, 0);
    lcd.write(barcha_cg);

  }else if (buffer_cg < 10){ 
    buffer_cg = buffer_cg + '0';
    lcd.print(buffer_cg) ;
  }else if (buffer_cg == 10){ 
    lcd.print(" ") ;
  }else if (buffer_cg == 11){ 
    lcd.print(":") ;
  }else if (buffer_cg == 12){ 
    lcd.print("%") ;
  }else {
    lcd.write(0xdf) ;
  }
}


void lcd_setup() {
  // Print a message to the LCD.
  // set up the LCD's number of columns and rows:

  lcd.begin(i2c_sda,i2c_scl); //ESP8266-01 I2C with pin 0-SDA 2-SCL
  lcd.backlight();
  lcd.clear();
  //CGRAMにたまごめ
  for(unsigned int i=0; i< 8; i++){
    lcd.createChar(i,lcd_cg[i]);
  }
}


void lcd_re_setup() {

  //CGRAMにたまごめ
  for(unsigned int i=0; i< 5; i++){
    lcd.createChar(i,lcd_cg[i]);
  }
  for(unsigned int i=7; i< 8; i++){
    lcd.createChar(i,lcd_cg[i]);
  }

}
