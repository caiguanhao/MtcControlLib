#pragma once
#include "device.h"

EXPORT DEV_HANDLE init();
EXPORT DEV_HANDLE initDev();
EXPORT int deInit(DEV_HANDLE dev);
EXPORT int deInitDev(DEV_HANDLE dev);
EXPORT int connectSerial(DEV_HANDLE dev, const char* portName);
EXPORT int connectCamera(DEV_HANDLE dev, const char* mediaName);
EXPORT int disconnectSerial(DEV_HANDLE dev);
EXPORT int disconnectCamera(DEV_HANDLE dev);
EXPORT int enumDevice(char *videoDevBuf, int videoDevSize,char *serialBuf, int serialSize);
