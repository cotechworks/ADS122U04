#include "ADS122U04.h"

// ピン設定
#define DRDY_PIN D0

// ステータス
enum AdcState {
  WAIT = 0,
  READ,
  OUT,
};

// 変数
ADS122U04 adc;
long data = 0;
AdcState state = WAIT;

// 変換完了信号による外部割り込み
void Interrupt() {
  if (state == WAIT) {
    state = READ;
  }
}

void setup() {
  // GPIOの初期化
  pinMode(DRDY_PIN, INPUT);
  attachInterrupt(DRDY_PIN, Interrupt, FALLING);

  // シリアル通信の初期化
  Serial.begin(200000);
  Serial1.begin(115200);

  // ADCの初期化
  adc.begin(Serial1, 115200);
}

void loop() {
  // AD変換結果の読み出し
  if (state == READ) {
    data = adc.readAdc();
    state = OUT;
  }

  // 結果の出力
  if (state == OUT) {
    Serial.println(data);
    state = WAIT;
  }
}