//esp-wroom-02 bme280 
// lcdナシ 
//10分毎に測定してAmbient-IoTに送信する。
//待機はDeepsleepを使う。(IO16ピンとRSTをショート)
//「ESP8266 core for Arduino」をバージョン2.7未満


#include <Wire.h>
#include <ESP8266WiFi.h>  
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#include <Ambient.h>
#include <BME280_MOD-1022.h>


//ADCはVCC電圧測定モード
ADC_MODE(ADC_VCC)

// ESP-WROOM-02が接続するWi-Fiアクセスポイントの設定
static const char *ssid[] = {
  "AAAAA","BBBBB","CCCCC","DDDDD","EEEEE"
  };

static const char *password[] = {
  "aaaaa","bbbbb","ccccc","ddddd","eeeee"
  };


// Ambientのチャネル設定
const char* userKey = "";
unsigned int channelId =  ;
const char* writeKey = "";


WiFiClient client;
Ambient ambient;

void setup() {
  Serial.begin(115200);
  Wire.begin(2,0);      //esp8266 esp-wroom-02
//  Wire.begin(4,5);      //sda,scl  wemos d1r2  A5,A4
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

//-------------------------
 // ステーションモードに設定
  WiFi.mode(WIFI_STA);
  WiFi.setOutputPower(20.5);
  
  // VCCを読み取る
  float esp_vcc = ESP.getVcc() ;  //mVで転送

//WIFI に接続
  ESP.wdtDisable();
  int ssid_no = wifi_connect();
  int rssi = WiFi.RSSI();
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
    delay(50);
    wdt_reset();
  }
  Serial.println("Done!");
  
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
  Serial.println(rssi);
        
  ambient.set(1, temperature);
  ambient.set(2, ssid_no);
  ambient.set(3, esp_vcc);
  ambient.set(4, humidity);
  ambient.set(5, pressure);
  ambient.set(6, rssi);
  ambient.set(7, connect_count);

  if (ambient.send()){
    Serial.println("send true");
  }else{
    Serial.println("send false");
  }

  //deep sleep
  ESP.wdtDisable();
  deep_sleep_process();
  ESP.wdtDisable();
  delay(3000);
}

//----------------------------------------------------------
void deep_sleep_process(){
  //DEEP SLEEPモード突入命令
  Serial.println("DEEP SLEEP START!!");
  delay(100);

  //1:μ秒での復帰までのタイマー時間設定  2:復帰するきっかけの設定（モード設定）
  ESP.deepSleep(5945 * 100 * 1000 , WAKE_RF_DEFAULT);    //コネクト等に平均5.5秒かかる　10分毎
    
  delay(100);
    
  //deepsleepモード移行までのダミー命令
  delay(1000);

  //実際にはこの行は実行されない
  Serial.println("DEEP SLEEPing....");
}

//-----------------------------------------------------------
int wifi_connect() {
  int i;
  int ssid_no ;

  wifiMulti.cleanAPlist();
  Serial.print("sizeof(ssid)=");
  Serial.println(sizeof(ssid));
  Serial.print("sizeof(int)=");
  Serial.println(sizeof(int));  
  for (i = 0; i < (sizeof(ssid)/sizeof(int)); i++) {
    wifiMulti.addAP(ssid[i], password[i]);
  }

  Serial.print("Connecting Wifi...");
  for (i = 0; i < 30; i++) {
    wdt_reset();
    if (wifiMulti.run() == WL_CONNECTED) {
      Serial.println("_WiFi connected!");
      Serial.println(WiFi.SSID());
      Serial.print("IP address: ");
      Serial.print(WiFi.localIP());
      break;
    }else{
      Serial.print(i);
      Serial.print(".");
      delay(5000);
    }
  }

  connect_count = i;
  Serial.println("");
  ssid_no = 0;          //とりあえずwifiなしを代入
  String buf = String(WiFi.SSID());

  if (i<30){
    for (i = 0; i < (sizeof(ssid)/sizeof(int)); i++) {
      if (buf == ssid[i]){
        ssid_no = i+1;
        Serial.print("select ssid = ");
        Serial.println(ssid_no);
        break;
      }
    }
  }
  
  return ssid_no;
}
