#pragma once

#include "mtc_interface.h"

typedef void (*CallbackFun)(const char* input);
EXPORT void SetCallBackFun(CallbackFun callbackfun);
EXPORT void test();


EXPORT int addImageByCamera(DEV_HANDLE dev, const char* imageId, uint imageIdLength);
EXPORT int addImageByCameraImage(DEV_HANDLE dev, const char* imageId, uint imageIdLength, char* ret, uint ret_size);
EXPORT int addImageByPc(DEV_HANDLE dev, const char* imageId, uint imageIdLength, char* data, uint length);
//EXPORT int uploadAddImageByPcSlice(DEV_HANDLE dev, const char* imageId, uint imageIdLength, char* ImageData, uint imageLength);
//EXPORT int checkUploadImageState(DEV_HANDLE dev);

EXPORT int uploadAddImageSlice(DEV_HANDLE dev, const char* imageId, uint imageIdLength, char *sliceData, uint sliceDataLength,int position,uint imageTotalLength,int islastSlice);

EXPORT int getDeviceFaceID(DEV_HANDLE dev, char* ret, uint ret_buf_size);
EXPORT int deleteImage(DEV_HANDLE dev, int mode, const char* imageId, uint imageIdLength);
EXPORT int queryId(DEV_HANDLE dev, int mode, const char* imageId, uint imageIdLength);
EXPORT int inputImage(DEV_HANDLE dev, int mode, char* data, uint length, char* ret, uint ret_size);
EXPORT int addFeature(DEV_HANDLE dev, const char* featureId, uint featureIdLength, char* data, uint length);
EXPORT int queryFeature(DEV_HANDLE dev, const char* featureId, uint featureIdLength, char* ret, uint ret_size);
EXPORT int startOnetoNumRecognize(DEV_HANDLE dev, int reomode, int mulmode);
EXPORT int startOnetoOneRecognize(DEV_HANDLE dev, int reomode, int mulmode, char* data, uint length);
EXPORT int resumeRecognize(DEV_HANDLE dev);
EXPORT int pauseRecognize(DEV_HANDLE dev);
EXPORT int queryRecognize(DEV_HANDLE dev);
EXPORT int setReoconfig(DEV_HANDLE dev, int mode, char* jsonData, uint jsonDataLength);
EXPORT int getReoconfig(DEV_HANDLE dev, int mode, char* ret, uint ret_buf_size);
EXPORT int openAutoUploadFaceInfoInFrame(DEV_HANDLE dev);
EXPORT int closeAutoUploadFaceInfoInFrame(DEV_HANDLE dev);
EXPORT int getDeviceFaceLibraryNum(DEV_HANDLE dev, char* ret, uint ret_buf_size);
EXPORT int ping(DEV_HANDLE dev, const char* data, int length);

EXPORT int uploadPackageSlice(DEV_HANDLE dev, int position, char *data, uint length);
EXPORT int applyUpgrade(DEV_HANDLE dev, const char* hash, int length);

//EXPORT int uploadPackage(DEV_HANDLE dev, int seek, const char* data, uint length);
//EXPORT void cancelUpload(DEV_HANDLE dev);
//EXPORT int checkUploadState(DEV_HANDLE dev);

EXPORT int getSysVer(DEV_HANDLE dev, char* ret, uint ret_buf_size);
EXPORT int reboot(DEV_HANDLE dev, int mode);
EXPORT int setUmodeToEngin(DEV_HANDLE dev);
EXPORT int reset(DEV_HANDLE dev);
EXPORT int recovery(DEV_HANDLE dev, int mode);
EXPORT int getDevSn(DEV_HANDLE dev, int mode, char* ret, uint ret_buf_size);
EXPORT int getDevModel(DEV_HANDLE dev, char* ret, uint ret_buf_size);
EXPORT int setUvcSwitch(DEV_HANDLE dev, int mode);
EXPORT int setCameraStream(DEV_HANDLE dev, int disposeMode, int cameraMode);
EXPORT int getCameraStream(DEV_HANDLE dev, int cameraMode, char* ret, uint ret_buf_size);
EXPORT int switchCamRgbIr(DEV_HANDLE dev, int mode);
EXPORT int setIRlight(DEV_HANDLE dev, int luminance);
EXPORT int getIRlight(DEV_HANDLE dev, char* ret, uint ret_buf_size);
EXPORT int setFrameRate(DEV_HANDLE dev, int frame_rate);
EXPORT int getFrameRate(DEV_HANDLE dev, char* ret, uint ret_buf_size);
EXPORT int setResolution(DEV_HANDLE dev, int mode);
EXPORT int setScreenDirection(DEV_HANDLE dev, int mode);
EXPORT int getScreenDirection(DEV_HANDLE dev, char* ret, uint ret_buf_size);
EXPORT int setRotateAngle(DEV_HANDLE dev, int mode);
EXPORT int setDeviceNoFlickerHz(DEV_HANDLE dev, int camera_id, int Hz, int enabled);
EXPORT int getDeviceNoFlickerHz(DEV_HANDLE dev, int camera_id, char* ret, uint ret_buf_size);
EXPORT int getLuminousSensitivityThreshold(DEV_HANDLE dev, char* ret, uint ret_buf_size);
EXPORT int getModuleAppVersion(DEV_HANDLE dev, char* ret, uint ret_buf_size);
EXPORT int setStreamFormat(DEV_HANDLE dev, int mode);
