#include "ADS122U04.h"

ADS122U04::ADS122U04() {
    m_reg[0] = 0x00;  // AINP = AIN0、AINN = AIN1 / ゲイン = 1 / PGA 有効
    m_reg[1] = 0xD8;  // 2000 SPS / ターボモード / 電圧:内部リファレンス(2.048V)
                      // / 温度センサーモード無効
    m_reg[2] = 0x00;  // 変換カウンター無効 / CRCチェック無効 /
                      // バーンアウト電流源オフ / IDACオフ
    m_reg[3] = 0x00;  // IDAC1 無効 / IDAC2 無効 / 自動データ読み取りモード
    m_reg[4] = 0x48;  // GPIO2=DRDY
}

void ADS122U04::setErrorInfo(ErrorState error) {
    m_error = String(error) + "-" + String(millis());
}

void ADS122U04::begin(Stream& stream, int baudrate) {
    m_serial = &stream;
    m_baudrate = baudrate;
    m_waitTimeUs = ((1000000 * 11) / m_baudrate);
    m_serial->setTimeout(2);
    setErrorInfo(NO_ERROR);

    reset();
    setConfig();
    start();
}

bool ADS122U04::setConfig() {
    for (int i = 0; i < 5; i++) {
        if (writeRegister(i, m_reg[i]) == false) {
            return false;
        }
    }
    return true;
}

void ADS122U04::SendCommand(uint8_t cmd) {
    m_serial->write(0x55);  // 同期コマンド
    m_serial->write(cmd);   // コマンド送信
}

void ADS122U04::start() {
    // スタートコマンド
    SendCommand(0x08);
}

void ADS122U04::reset() {
    // リセットコマンド
    SendCommand(0x06);
    // リセット完了待ち(最低でも80us以上待つ)
    delay(1);
}

String ADS122U04::checkError() {
    return m_error;
}

long ADS122U04::readAdc() {
    // 手動データ読み取りモードなら結果読み出しコマンドを送信
    if((m_reg[3] & 0x01) == 0){
        SendCommand(0x16);
    }

    // レスポンスの受信
    char data[3];
    long vol = 0;
    int size = m_serial->readBytes(data, 3);

    if(size == 3){
        vol = data[0] + (data[1] << 8) + (data[2] << 16);
    }else{
        setErrorInfo(ADC_READ_ERROR);
        return 0;
    }

    // 受信結果を返す
    return vol;
}

bool ADS122U04::writeRegister(int address, int data) {
    // 引数チェック
    if (address < 0 || address > 4) {
        setErrorInfo(WRITE_REGISTER_ADDRESS_ERROR);
        return false;
    }
    if (data < 0 || data > 255) {
        setErrorInfo(WRITE_REGISTER_DATA_ERROR);
        return false;
    }

    // レジスタ書き込みコマンド
    SendCommand(0x40 | (address << 1));
    m_serial->write(data);
    return true;
}

int ADS122U04::readRegister(int address) {
    // 引数チェック
    if (address < 0 || address > 4) {
        setErrorInfo(READ_REGISTER_ADDRESS_ERROR);
        return 0;
    }

    // レジスタ読み出しコマンド
    SendCommand(0x20 | (address << 1));

    // レスポンスの受信
    char data[1];
    int size = m_serial->readBytes(data, 1);

    if(size != 1){
        setErrorInfo(READ_REGISTER_ERROR);
        return 0;
    }

    // 受信結果を返す
    return data[0];
}