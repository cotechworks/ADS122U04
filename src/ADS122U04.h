#pragma once

#include <Arduino.h>

enum OperatingMode{
    SINGLE = 0,
    CONTINUOUS,
};

enum ErrorState{
    NO_ERROR = 0,
    ADC_READ_ERROR,
    WRITE_REGISTER_ADDRESS_ERROR,
    WRITE_REGISTER_DATA_ERROR,
    READ_REGISTER_ADDRESS_ERROR,
    READ_REGISTER_ERROR,
};

class ADS122U04 {
   public:
    ADS122U04();
    void begin(Stream& stream, int baudrate);
    bool setConfig();
    void SendCommand(uint8_t cmd);
    void start();
    void reset();
    String checkError();
    long readAdc();
    bool writeRegister(int address,int data);
    int readRegister(int address);
    void setErrorInfo(ErrorState error);

   private:
    Stream* m_serial;       // 通信に使用するシリアル
    int m_baudrate;         // ボーレート
    int m_waitTimeUs;       // 送信時間(1バイト+スタートビット+ストップビット+予備1ビット)
    String m_error;         // エラー情報
    OperatingMode m_mode;   // 動作モード
    uint8_t m_reg[5];       // config情報のバッファ

    
};