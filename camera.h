#pragma once
extern "C"
{
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavutil/time.h>
#include <libavutil/log.h>
};
#include "stdafx.h"

#define DECODING_MODE_MJPEG 0x00
#define DECODING_MODE_H264  0x01

class Camera {
private:
	AVFormatContext *m_ic = nullptr;
	AVCodec *m_vcodec = nullptr;
	AVCodecContext *m_vc = nullptr;
	AVPacket *m_pkt = nullptr;
	AVFrame *m_frame = nullptr;
	SwsContext *m_vctx = nullptr;

	std::mutex devio_lock;
	bool isPlay = false;
	std::mutex frame_lock;

	char errorbuf[1024];

	bool initPkt();
	bool readFrame();
	void parseFrame();

	void loaderThread();
	std::thread loader;
	bool isexit = false;

public:
	Camera();
	~Camera();

	uchar* frameImg = nullptr;
	char verifyMsg[1024];
	int verifyMsgSize = 0;
	bool isNewFrame = false;
	bool isNewVerify = false;

	int width = 1280;
	int height = 720;
	void SetResolution(int width, int height) { this->width = width; this->height = height; }
	string GetResolution();

	int FrameSize() { return height * width * 4; }

	int framerate = 30;
	void SetFramerate(int framerate) { this->framerate = framerate; };
	string GetFramerate();

	int decoding = DECODING_MODE_MJPEG;
	void SetDecoing(int mode) { decoding = mode; }
	string GetDecoding();

	bool isCameraConnected() { return this->isPlay; }
	string cameraName;
	int ConnectCamera(const char* cameraName);
	void CloseCamera();

	int GetFrame(char* imageBuf, int imageSize, char* verifyBuf, int verifySize);
};
