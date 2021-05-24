#include "Serial.h"
#include "stdafx.h"


Serial::Serial()
{
    qDebug()<<"Serial()";
    m_serial = new QSerialPort();
}

Serial::~Serial() {
    if (this->connected) {
        this->connected = false;
        this->closeSerial();
    }
    if(m_serial != nullptr)
    {
        delete m_serial;
    }


}

void Serial::connectSerial(const char *portName) {
    if (this->isConnected())
    {
        this->closeSerial();
    }
    qDebug()<<"m_portName = portName;";
    m_portName = portName;
    qDebug()<<"m_serial->setPortName(m_portName);";
    m_serial->setPortName(m_portName);
    m_serial->setBaudRate(QSerialPort::Baud115200);
    m_serial->setDataBits(QSerialPort::Data8);
    m_serial->setParity(QSerialPort::NoParity);
    m_serial->setStopBits(QSerialPort::OneStop);
    m_serial->setFlowControl(QSerialPort::NoFlowControl);
    qDebug()<<"m_serial->setFlowControl(QSerialPort::NoFlowControl);";
    if (m_serial->open(QIODevice::ReadWrite)) {
        this->connected = true;
        qDebug()<<"connectSerial<true";
    } else {
        this->connected = false;
        qDebug()<<"connectSerial<false";
    }
}
void Serial::closeSerial()
{
    if (m_serial->isOpen())
    {
        m_serial->close();
        this->connected = false;
    }
}

bool Serial::isConnected()
{
    return this->connected;
}

int Serial::writeDate(const char *data, int maxSize)
{
    int result = -1;
    if(m_serial->isOpen())
    {
        QByteArray nn(data,maxSize);
        result = m_serial->write(data,maxSize);
//        qDebug()<<"--------- [sendData] " << nn.toHex().left(80);
//        qDebug() << "   maxsize " << maxSize << "    result " << result;
    }
    return result;
}

bool Serial::waitForBytesWritten(int msecs)
{
    return m_serial->waitForBytesWritten(msecs);
}
bool Serial::waitForReadyRead(int msecs)
{
    return m_serial->waitForReadyRead(msecs);
}
QByteArray Serial::readAll()
{
    return m_serial->readAll();
}
qint64 Serial::read(char *data, qint64 maxlen)
{
    return m_serial->read(data,maxlen);
}
