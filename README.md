# esp-wroom-02 bme280 ambient-iot

センサ(温度・湿度等)データをAmbient-IoTにエスカレーションする。


・ESP-WROOM-02　BME280　シリアルLCD　Ambient-IoT

ESP-BME280_LCD.ino

・LCDナシ、低消費電力(DeepSleep)

　条件が良いと単三電池2本で1ヶ月くらい持ちます

ESP-BME280.ino

・フリーWIFIを設定して、車に乗せたりする用

ESP-BME280_history.ino





# 構成
・ ESP-WROOM-02(または類似品)

・ BME280(I2Cを使います)

・ LCD　SC2004(シリアルIF)、SC1602(シリアルIF)　無くても良い

・単三電池2本(この電圧でも十分動きます)

  
https://github.com/AmbientDataInc/Ambient_ESP8266_lib

https://github.com/TakehikoShimojima/Ambient_ESP8266_lib
  
 
