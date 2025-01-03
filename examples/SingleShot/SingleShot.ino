#include "ADS122U04.h"

ADS122U04 adc;

void setup() {
    // シリアル通信の初期化
    Serial.begin(115200);
    Serial1.begin(115200);

    // ADCの初期化
    adc.begin(Serial1, 115200);
}

void loop() {
    // 結果読み出し
    long data = adc.readAdc();
    // シリアル通信で出力
    Serial.println(data);

    delay(1000);
}