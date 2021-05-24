#include "camapi.h"

EXPORT int getFrame(DEV_HANDLE dev, char* imageBuf, int imageSize, char* verifyBuf, int verifySize) {
	return dev->GetFrame(imageBuf, imageSize, verifyBuf, verifySize);
}

EXPORT int getFrameWidth(DEV_HANDLE dev) {
//    qDebug()<<"getFrameWidth:"<<dev->width;
	return dev->width;
}

EXPORT int getFrameHeight(DEV_HANDLE dev) {
//    qDebug()<<"getFrameHeight:"<<dev->height;
	return dev->height;
}

class videoScope {
private:
	DEV_HANDLE dev;
	bool closedByThis = false;
public:
	explicit videoScope(DEV_HANDLE dev) {
		this->dev = dev;
		if (dev->isCameraConnected()) {
			dev->CloseCamera();
			closedByThis = true;
		}
	}
	~videoScope() {
		if (closedByThis) dev->ConnectCamera(dev->cameraName.c_str());
	}
};

EXPORT int SetResolution(DEV_HANDLE dev, int width, int height) {
	videoScope scope(dev);
	dev->SetResolution(width, height);
	return 0;
}

EXPORT int SetFramerate(DEV_HANDLE dev, int framerate) {
	videoScope scope(dev);
	dev->SetFramerate(framerate);
	return 0;
}

EXPORT int SetDecoding(DEV_HANDLE dev, int mode) {
	videoScope scope(dev);
	dev->SetDecoing(mode);
	return 0;
}
