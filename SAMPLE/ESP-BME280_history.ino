//esp-wroom-02 bme280 
// lcdナシ 
//10分毎に測定してAmbient-IoTに送信する。
//待機はDeepsleepを使う。(IO16ピンとRSTをショート)
//「ESP8266 core for Arduino」をバージョン2.7未満
//wifiが無い時は、S-RAMに貯める


#include <Wire.h>
#include <ESP8266WiFi.h>  
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;

#include <Ambient.h>
#include <user_interface.h>
#include <time.h>
#include <OneWire.h>    #crc計算用

// BME280 MOD-1022 weather multi-sensor Arduino demo using a SAMD21 board
// Written originally by Embedded Adventures
#include <BME280_MOD-1022.h>


//ADCはVCC電圧測定モード
ADC_MODE(ADC_VCC)

// ESP8266が接続するWi-Fiアクセスポイントの設定
static const char *ssid[14] = {
  "AAAAA","BBBBB","CCCCC","DDDDD","EEEEE",
  "FFFFF","GGGGG","HHHHH","IIIII","JJJJJ",
  "KKKKK","LLLLL","MMMMM","NNNNN"
  };

static const char *password[14] = {
  "aaaaa","bbbbb","ccccc","ddddd","eeeee",
  "","","","","",
  "","","",""
  };


// Ambientのチャネル設定
const char* userKey = "";
unsigned int channelId =  ;
const char* writeKey = "";

//ambient関係
//USER_DATA_ADDR = 64
//64 crcとか保存したデータ数とか
//65～89 前回の値を読み出す
//66～90 さっき読んだ前回の値を保存(1つづつづれさす）
//6 今回の値を保存

static const uint32_t USER_DATA_ADDR = 64;

static struct {
  unsigned long wrote_number;
  unsigned long histry_crc;
} retain_data;


unsigned int ssid_no ;

#define bufside  25

static struct {
  unsigned long dateandtime[bufside]; 
  float temperature[bufside];
  float  humidity[bufside];
  float  pressure[bufside];
  unsigned long ssid_vcc_rssi[bufside];
} retain_data3;


#define BUFSIZE  17000
char buffer[BUFSIZE]; 
Ambient ambient;
int sent;


WiFiClient client;

void setup() {
  Serial.begin(115200);
  Wire.begin(2,0);      //esp8266 esp-wroom-02
//  Wire.begin(4,5);      //sda,scl  wemos d1r2  A4,A5
//  Wire.begin(12,13);  //wemos d1r2  mosi_d7,miso_d6
  Serial.println("");
  Serial.println("BME280 ESP-WROOM-02 START");
  Serial.print("channelId = ");
  Serial.println(channelId);

  uint8_t chipID = BME280.readChipId();
  Serial.print("BME280 Chip ID: 0x");
  Serial.println(chipID, HEX);

  delay(100);
}

void loop() {
  unsigned int lo_wrote_number;
  unsigned long start_time = millis();
  struct tm *tm;
  time_t t;
  Serial.begin(115200); 
//-------------------------
 // ステーションモードに設定
  WiFi.mode(WIFI_STA);

//WIFI に接続
  ESP.wdtDisable();
  wifi_connect();
  configTime(0, 0,  "ntp.nict.jp", "time.google.com");//NTPの設定
  Serial.print("ssid_no=");
  Serial.println(ssid_no);

// VCCを読み取る
  float esp_vcc = ESP.getVcc() ;  //mVで転送
  int i_rssi = WiFi.RSSI();
  Serial.print("int rssi=");
  Serial.println(i_rssi);
  
  unsigned char rssi = (unsigned char)(abs(i_rssi));
  ambient.begin(channelId, writeKey, &client);
 

  // need to read the NVM compensation parameters
  BME280.readCompensationParams();
  
  // Need to turn on 1x oversampling, default is os_skipped, which means it doesn't measure anything
  BME280.writeOversamplingPressure(os1x);  // 1x over sampling (ie, just one sample)
  BME280.writeOversamplingTemperature(os1x);
  BME280.writeOversamplingHumidity(os1x);
  
  // example of a forced sample.  After taking the measurement the chip goes back to sleep
  BME280.writeMode(smForced);
  Serial.print("Measuring...");
  int i = 0;
  while (BME280.isMeasuring()) {
    i=i+1;
    if (i==20){
      break;
    }
    Serial.print(".");
    delay(500);
    wdt_reset();
  }
  Serial.println("Done!");
  //WiFi.printDiag( Serial );
  // BME280から測定値を読み取る
  BME280.readMeasurements();
  float temperature = BME280.getTemperature();  // must get temp first
  float humidity = BME280.getHumidity();
  float pressure = BME280.getPressure();

  Serial.print("Temp=");
  Serial.println(temperature);
  Serial.print("Humidity=");
  Serial.println(humidity);
  Serial.print("Pressure=");
  Serial.println(pressure);
  Serial.print("Vcc=");
  Serial.println(esp_vcc);
  Serial.print("RSSI=");
  Serial.println(-1*rssi);

//bulkの検討
  bool ok;

  for (i = bufside-1; i > 1; i--) {
    retain_data3.dateandtime[i] = retain_data3.dateandtime[i-1];
    retain_data3.temperature[i] = retain_data3.temperature[i-1];
    retain_data3.humidity[i] = retain_data3.humidity[i-1];
    retain_data3.pressure[i] = retain_data3.pressure[i-1];
    retain_data3.ssid_vcc_rssi[i] = retain_data3.ssid_vcc_rssi[i-1];
  }

  Serial.print("read:");
  Serial.println(USER_DATA_ADDR+1);  
  Serial.print("size:");
  Serial.println(sizeof(retain_data3));
 
  for (i = 0; i < bufside; i++) {
    Serial.print("count");
    Serial.println(i);
    Serial.println(retain_data3.dateandtime[i]); 
    Serial.println(retain_data3.temperature[i]);
    Serial.println((unsigned char)(retain_data3.ssid_vcc_rssi[i]/16777216));
    Serial.println((unsigned short)((retain_data3.ssid_vcc_rssi[i]/256)%65536));
    Serial.println(retain_data3.humidity[i]);
    Serial.println(retain_data3.pressure[i]);
    Serial.println(-1*(unsigned char)(retain_data3.ssid_vcc_rssi[i]%256));
    Serial.println("");
  }

  retain_data3.temperature[1] = temperature;
  retain_data3.humidity[1] = humidity;
  retain_data3.pressure[1]= pressure;

  if (ssid_no == 0) {                     //wifiが無かったら、前回+600秒    
    ssid_no = 19;          //デバック
    t = retain_data3.dateandtime[2] + 720 + (((unsigned long)millis() - start_time)/1000);    
  }else{
    for ( i = 0; i < 21; i++) {
      wdt_reset();
      t = time(NULL);
      Serial.print("t:");    
      Serial.println(t);
      if (t > 1635914704){
        break;
      }   
      delay(500);
    }
    if (i>=20){
      //ssid_no = 0;          //時間同期出来なかったらwifiなしと見なす、rssiは残す
      ssid_no = ssid_no + 20;          //デバック
      t = retain_data3.dateandtime[2] + 600 + (((unsigned long)millis() - start_time)/1000);
    }
  }
  retain_data3.dateandtime[1] = (unsigned long)t; 
  retain_data3.ssid_vcc_rssi[1]=(((((unsigned char)ssid_no)*16777216)&4278190080) |((((unsigned short)esp_vcc)*256)&16776960) |((unsigned char)(rssi)));
  
  retain_data.wrote_number++;
  lo_wrote_number = retain_data.wrote_number;
  if (lo_wrote_number > bufside){
    lo_wrote_number = bufside ;
  }
  if (retain_data.wrote_number > 253){
    retain_data.wrote_number = 253 ;
  }
  
  Serial.print("retain_data.wrote_number:");
  Serial.println(lo_wrote_number);

  Serial.print("write:");
  Serial.println(USER_DATA_ADDR+2);
  for (i = 0; i < lo_wrote_number+1; i++) {
    Serial.print("count");
    Serial.println(i);
    Serial.println(retain_data3.dateandtime[i]); 
    Serial.println(retain_data3.temperature[i]);
    Serial.println((unsigned char)(retain_data3.ssid_vcc_rssi[i]/16777216));
    Serial.println((unsigned short)((retain_data3.ssid_vcc_rssi[i]/256)%65536));
    Serial.println(retain_data3.humidity[i]);
    Serial.println(retain_data3.pressure[i]);
    Serial.println(-1*(unsigned char)(retain_data3.ssid_vcc_rssi[i]%256));
  }

  sprintf(buffer, "{\"writeKey\":\"%s\",\"data\":[", writeKey);
  for (i = 1; i < lo_wrote_number+1; i++) {
    sprintf(&buffer[strlen(buffer)], "{\"created\":%d000,\"d1\":%f,\"d2\":%d,\"d3\":%d,\"d4\":%f,\"d5\":%f,\"d6\":%d,\"d7\":%d},",retain_data3.dateandtime[i],retain_data3.temperature[i],(unsigned char)(retain_data3.ssid_vcc_rssi[i]/16777216),(unsigned short)((retain_data3.ssid_vcc_rssi[i]/256)%65536),(float)retain_data3.humidity[i],(float)retain_data3.pressure[i],-1*(unsigned char)(retain_data3.ssid_vcc_rssi[i]&255),lo_wrote_number);
  }
  buffer[strlen(buffer)-1] = '\0';
  sprintf(&buffer[strlen(buffer)], "]}\r\n");

  Serial.print("buf: ");
  Serial.print(strlen(buffer));
  Serial.print(" bytes\r\n");

  if ((ssid_no == 0)|(ssid_no >= 19)) {                     //wifiが無い時は送信しない debug
    Serial.println("not sent");
  }else{
    sent = ambient.bulk_send(buffer );
    if (strlen(buffer) == sent){
      retain_data.wrote_number = 0 ;
    }else{
      ssid_no = ssid_no + 100;          //デバック
      retain_data3.dateandtime[1] = (unsigned long)t; 
      retain_data3.ssid_vcc_rssi[1]=(((((unsigned char)ssid_no)*16777216)&4278190080) |((((unsigned short)esp_vcc)*256)&16776960) |((unsigned char)(rssi)));
    }
  }

  Serial.print("sent: ");
  Serial.print(sent);
  Serial.print("\r\n");
  Serial.print("buffer: ");
  Serial.print(buffer);
  Serial.print("\r\n");

  //deep sleep
  ESP.wdtDisable();
  deep_sleep_process();
  ESP.wdtDisable();
  delay(3000);
}

//----------------------------------------------------------
void deep_sleep_process(){
  bool ok;
  byte byte_buffer[4];

  retain_data3.dateandtime[0] = retain_data.wrote_number;
  retain_data3.temperature[0] = (float)ssid_no; ;
  retain_data3.humidity[0]    = (float)sent ;
  retain_data3.pressure[0]    = (float)channelId ;

  byte_buffer[0] = (byte)(retain_data3.dateandtime[0]);
  byte_buffer[1] = (byte)(retain_data3.temperature[0]);
  byte_buffer[2] = (byte)(retain_data3.humidity[0]);
  byte_buffer[3] = (byte)(retain_data3.pressure[0]);

  retain_data.histry_crc = (unsigned long)(OneWire::crc8(byte_buffer,4));
  retain_data3.ssid_vcc_rssi[0] = retain_data.histry_crc;
 
  ok = system_rtc_mem_write(USER_DATA_ADDR, &retain_data3, sizeof(retain_data3));

  if (!ok) {
    Serial.println("system_rtc_mem_write failed");
  }
  //DEEP SLEEPモード突入命令
  Serial.println("DEEP SLEEP START!!");
  delay(100);

  //1:μ秒での復帰までのタイマー時間設定  2:復帰するきっかけの設定（モード設定）
  ESP.deepSleep(5945 * 100 * 1000 , WAKE_RF_DEFAULT);    //コネクト等に平均5.5秒かかる　10分毎
    
  delay(100);
  Serial.println(WiFi.SSID());
  //deepsleepモード移行までのダミー命令
  delay(1000);
    for (int i = 0; i < 600; i++) {
      wdt_reset();
      delay(1000);
    }

  //デバック用
  ESP.restart();
  Serial.println("DEEP SLEEPing....");
}


//-----------------------------------------------------------
int wifi_connect() {
  int i, loop;
  int stat;
  bool ok;
  byte byte_buffer[4];

  ok = system_rtc_mem_read(USER_DATA_ADDR, &retain_data3, sizeof(retain_data3));
  if (!ok) {
    Serial.println("system_rtc_mem_read failed");
  }

  retain_data.wrote_number = retain_data3.dateandtime[0];
  retain_data.histry_crc = retain_data3.ssid_vcc_rssi[0];

  byte_buffer[0] = (byte)(retain_data3.dateandtime[0]);
  byte_buffer[1] = (byte)(retain_data3.temperature[0]);
  byte_buffer[2] = (byte)(retain_data3.humidity[0]);
  byte_buffer[3] = (byte)(retain_data3.pressure[0]);

  if ((OneWire::crc8(byte_buffer,4)) != (byte)retain_data.histry_crc){
    retain_data.wrote_number = 0;
  }

  wifiMulti.cleanAPlist();

  for (int i = 1; i < 15; i++) {
    wifiMulti.addAP(ssid[i-1], password[i-1]);
  }

  Serial.println("Connecting Wifi...");
  for (int i = 0; i < 50; i++) {
    wdt_reset();
    if (wifiMulti.run() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
    }else{
      Serial.println("_WiFi connected!");
      Serial.println(WiFi.SSID());
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      break;
    }
  }

  ssid_no = 0;          //とりあえずwifiなしを代入
  String buf = String(WiFi.SSID());

  if (i<50){
    for (int i = 1; i < 15; i++) {
      if (buf == ssid[i-1]){
        ssid_no = (unsigned char)i;
        Serial.print("select ssid = ");
        Serial.println(ssid_no);
        break;
      }
    }
  }
  return ssid_no;
}
