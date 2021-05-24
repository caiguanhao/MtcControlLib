extern "C" {
    // camapi.h
    int getFrame(void* dev, char* imageBuf, int imageSize, char* verifyBuf, int verifySize);
    int getFrameWidth(void* dev);
    int getFrameHeight(void* dev);

    // devapi.h
    void* init();
    void* initDev();
    int deInit(void* dev);
    int deInitDev(void* dev);
    int connectSerial(void* dev, const char* portName);
    int connectCamera(void* dev, const char* mediaName);
    int disconnectSerial(void* dev);
    int disconnectCamera(void* dev);
    int enumDevice(char *videoDevBuf, int videoDevSize, char *serialBuf, int serialSize);

    // commands/data/mtc_export.h
    int addImageByCamera(void* dev, const char* imageId, unsigned int imageIdLength);
    int addImageByCameraImage(void* dev, const char* imageId, unsigned int imageIdLength, char* ret, unsigned int ret_size);
    int addImageByPc(void* dev, const char* imageId, unsigned int imageIdLength, char* data, unsigned int length);
    int uploadAddImageSlice(void* dev, const char* imageId, unsigned int imageIdLength, char *sliceData, unsigned int sliceDataLength,int position,unsigned int imageTotalLength,int islastSlice);
    int getDeviceFaceID(void* dev, char* ret, unsigned int ret_buf_size);
    int deleteImage(void* dev, int mode, const char* imageId, unsigned int imageIdLength);
    int queryId(void* dev, int mode, const char* imageId, unsigned int imageIdLength);
    int inputImage(void* dev, int mode, char* data, unsigned int length, char* ret, unsigned int ret_size);
    int addFeature(void* dev, const char* featureId, unsigned int featureIdLength, char* data, unsigned int length);
    int queryFeature(void* dev, const char* featureId, unsigned int featureIdLength, char* ret, unsigned int ret_size);
    int startOnetoNumRecognize(void* dev, int reomode, int mulmode);
    int startOnetoOneRecognize(void* dev, int reomode, int mulmode, char* data, unsigned int length);
    int resumeRecognize(void* dev);
    int pauseRecognize(void* dev);
    int queryRecognize(void* dev);
    int setReoconfig(void* dev, int mode, char* jsonData, unsigned int jsonDataLength);
    int getReoconfig(void* dev, int mode, char* ret, unsigned int ret_buf_size);
    int openAutoUploadFaceInfoInFrame(void* dev);
    int closeAutoUploadFaceInfoInFrame(void* dev);
    int getDeviceFaceLibraryNum(void* dev, char* ret, unsigned int ret_buf_size);
    int ping(void* dev, const char* data, int length);
    int uploadPackageSlice(void* dev, int position, char *data, unsigned int length);
    int applyUpgrade(void* dev, const char* hash, int length);

    int getSysVer(void* dev, char* ret, unsigned int ret_buf_size);
    int reboot(void* dev, int mode);
    int setUmodeToEngin(void* dev);
    int reset(void* dev);
    int recovery(void* dev, int mode);
    int getDevSn(void* dev, int mode, char* ret, unsigned int ret_buf_size);
    int getDevModel(void* dev, char* ret, unsigned int ret_buf_size);
    int setUvcSwitch(void* dev, int mode);
    int setCameraStream(void* dev, int disposeMode, int cameraMode);
    int getCameraStream(void* dev, int cameraMode, char* ret, unsigned int ret_buf_size);
    int switchCamRgbIr(void* dev, int mode);
    int setIRlight(void* dev, int luminance);
    int getIRlight(void* dev, char* ret, unsigned int ret_buf_size);
    int setFrameRate(void* dev, int frame_rate);
    int getFrameRate(void* dev, char* ret, unsigned int ret_buf_size);
    int setResolution(void* dev, int mode);
    int setScreenDirection(void* dev, int mode);
    int getScreenDirection(void* dev, char* ret, unsigned int ret_buf_size);
    int setRotateAngle(void* dev, int mode);
    int setDeviceNoFlickerHz(void* dev, int camera_id, int Hz, int enabled);
    int getDeviceNoFlickerHz(void* dev, int camera_id, char* ret, unsigned int ret_buf_size);
    int getLuminousSensitivityThreshold(void* dev, char* ret, unsigned int ret_buf_size);
    int getModuleAppVersion(void* dev, char* ret, unsigned int ret_buf_size);
    int setStreamFormat(void* dev, int mode);

    // consts.h
    bool LibVer(char* ver,int size);
}
