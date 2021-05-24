#pragma once
#include "device.h"


EXPORT int getFrame(DEV_HANDLE dev, char* imageBuf, int imageSize, char* verifyBuf, int verifySize);
EXPORT int getFrameWidth(DEV_HANDLE dev);
EXPORT int getFrameHeight(DEV_HANDLE dev);

EXPORT int SetResolution(DEV_HANDLE dev, int width, int height);
EXPORT int SetFramerate(DEV_HANDLE dev, int framerate);
EXPORT int SetDecoding(DEV_HANDLE dev, int mode);