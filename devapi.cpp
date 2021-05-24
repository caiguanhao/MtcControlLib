#include "devapi.h"

EXPORT DEV_HANDLE init() {
    std::cerr << "Init a dev handle" << std::endl;
    DEV_HANDLE dev = new Device;
    return dev;
}

EXPORT DEV_HANDLE initDev() {
    return init();
}

EXPORT int deInit(DEV_HANDLE dev) {
    if (dev == NULL) {
        return 1;
    }

    delete dev;
    return 0;
}

EXPORT int deInitDev(DEV_HANDLE dev) {
    return deInit(dev);
}

EXPORT int connectSerial(DEV_HANDLE dev, const char *portName) {
    dev->connectSerial(portName);
    return dev->isConnected() ? 0x00 : 0x01;
}

EXPORT int connectCamera(DEV_HANDLE dev, const char *mediaName) {
    int re = dev->ConnectCamera(mediaName);
    return re == 0 ? 0x00 : 0x01;
}

EXPORT int disconnectSerial(DEV_HANDLE dev) {
    dev->closeSerial();
    return 0;
}

EXPORT int disconnectCamera(DEV_HANDLE dev) {
    dev->CloseCamera();
    return 0;
}



EXPORT int enumDevice(char *videoDevBuf, int videoDevSize, char *serialBuf, int serialSize) {
    int ret = 0;
    memset(videoDevBuf, 0, videoDevSize);
    memset(serialBuf, 0, serialSize);
    QString videoDevBufQStr= "";
    QString serialBufQStr = "";

    const QList<QCameraInfo> camerasList = QCameraInfo::availableCameras();
//    qDebug() << "camerasList";
    for (const QCameraInfo &cameraInfo : camerasList)
    {
        ret = ret + 1;
#ifdef WindowsBuild
        videoDevBufQStr.append(cameraInfo.description());
        videoDevBufQStr.append("|");
#elif LinuxBuild
        videoDevBufQStr.append(cameraInfo.deviceName());
        videoDevBufQStr.append("|");

#endif
    }

    if(videoDevBufQStr.isEmpty() == false)
    {
        videoDevBufQStr = videoDevBufQStr.left(videoDevBufQStr.size()-1);
        strcpy(videoDevBuf,videoDevBufQStr.toLocal8Bit().data());
        qDebug() << " videoDevBufQStr " << videoDevBufQStr;
    }
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        serialBufQStr.append(info.portName());
        serialBufQStr.append("|");

    }
    if(serialBufQStr.isEmpty() == false)
    {
        serialBufQStr = serialBufQStr.left(serialBufQStr.size()-1);
        strcpy(serialBuf,serialBufQStr.toLocal8Bit().data());
        qDebug() << " serialBufQStr " << serialBuf;
    }

    return ret;
}
