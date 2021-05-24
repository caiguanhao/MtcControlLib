#pragma once
#define SERIAL_MAX_DATA_LENGTH 255
#define SERIAL_WAIT_TIME 100

//#include <mutex>
//#include <iostream>
//#include <stdexcept>

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QCameraInfo>


class Serial
{
public:
    Serial();
    void connectSerial(const char *portName);
    bool isConnected();
    void closeSerial();
    int writeDate(const char *data, int maxSize);
    bool waitForBytesWritten(int msecs);
    bool waitForReadyRead(int msecs);
    QByteArray readAll();
    qint64 read(char *data, qint64 maxlen);

    ~Serial();
private:
    bool connected = false;
    QSerialPort *m_serial = nullptr;
    QString m_portName="";

};
