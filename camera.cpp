#include "camera.h"
#include<QString>
#include<QDebug>//fanzhao
//#include <include/json.hpp>


//static int64_t lastReadPacktTime = 0;
//int interrupt_cb(void *ctx)
//{
//    int timeout = 10;
//    if(av_gettime() - lastReadPacktTime > timeout * 1000 *1000)
//    {
//        return -1;  //返回>=0时，avformat_open_input继续执行
//    }
//    return 0;
//}
Camera::Camera() {
    errorbuf[0] = '\0';
    isPlay = false;
}

Camera::~Camera() {
    CloseCamera();
}

string Camera::GetResolution() {
    ostringstream oss;
    oss << width << "x" << height;
    return oss.str();
}

string Camera::GetFramerate() {
    ostringstream oss;
    oss << framerate;
    return oss.str();
}

string Camera::GetDecoding() {
    string ret;
    switch (decoding) {
        case DECODING_MODE_H264:
            ret = "h264";
            break;
        case DECODING_MODE_MJPEG:
            ret = "mjpeg";
            break;
    }
    return ret;
}

int Camera::ConnectCamera(const char *deviceName) {
    CloseCamera();
    isexit = false;
    this->cameraName = deviceName;
    std::lock_guard<std::mutex> lock(devio_lock);

#ifndef NDEBUG
    av_log_set_level(AV_LOG_ERROR);
#else
    av_log_set_level(AV_LOG_QUIET);
#endif
    //注册所有的编解码器
    void *opaque = nullptr;
    av_demuxer_iterate(&opaque);

    //注册解码器
    avdevice_register_all();

#ifdef WIN32
    AVInputFormat *ifmt = av_find_input_format("dshow");
#else
    AVInputFormat *ifmt = av_find_input_format("v4l2");
#endif

    AVDictionary *options = nullptr;
//    av_dict_set(&options, "input_format", GetDecoding().c_str(), 0);
//    av_dict_set(&options, "framerate", GetFramerate().c_str(), 0);
//    av_dict_set(&options, "video_size", GetResolution().c_str(), 0);
    av_dict_set(&options, "input_format", "mjpeg", 0);
    av_dict_set(&options, "framerate", "30", 0);
    av_dict_set(&options, "video_size", "1280x720", 0);//必须设置为该值

#ifdef WIN32
    QString cameraName("video=");
#else
    QString cameraName;
#endif
    m_ic = avformat_alloc_context();
    m_ic->flags |= AVFMT_FLAG_NONBLOCK;
    av_dict_set(&options, "stimeout", "2000000", 0); //设置超时断开连接时间，单位微秒 doesn't work for dshow
    av_dict_set(&options, "max_delay", "500000", 0); //设置最大时延

//    m_ic->interrupt_callback.callback  =  interrupt_cb;  //函数指针，指向自定义中断函数
//    m_ic->interrupt_callback.opaque = this;  //在多实例的情况下，将当前指针传给他


    int re = avformat_open_input(
                &m_ic,
                cameraName.append(deviceName).toLatin1().data(),	//"video=UVC Camera",  Integrated Webcam
                ifmt,  // 0表示自动选择解封器
                &options //参数设置，比如rtsp的延时时间
                );
    if(re != 0)
    {
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf) - 1);
        cerr << "camera open failed! :" << buf << endl;
        return -1;
    }
    cerr << "camera open success! " << endl;

    //获取流信息
    re = avformat_find_stream_info(m_ic, nullptr);

    //总时长 毫秒
    int64_t totalMs = m_ic->duration / (AV_TIME_BASE / 1000);
    cerr << "totalMs = " << totalMs << endl;

    //音视频索引，读取时区分音视频
    int videoStream = 0;

    //获取音视频流信息 （遍历，函数获取）
    for (int i = 0; i < m_ic->nb_streams; i++)
    {
        AVStream *as = m_ic->streams[i];
        cerr << "codec_id = " << as->codecpar->codec_id << endl;
        cerr << "format = " << as->codecpar->format << endl;

        //视频 AVMEDIA_TYPE_VIDEO
        if (as->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            cerr << i << "视频信息" << endl;
            width = as->codecpar->width;
            height = as->codecpar->height;
            cerr << "width=" << as->codecpar->width << endl;
            cerr << "height=" << as->codecpar->height << endl;
        }
    }

    //获取视频流
    videoStream = av_find_best_stream(m_ic, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);

    //////////////////////////////////////////////////////////
    ///视频解码器打开
    ///找到视频解码器
    AVCodec *vcodec = avcodec_find_decoder(m_ic->streams[videoStream]->codecpar->codec_id);//ic->streams[videoStream]->codecpar->codec_id   //AV_CODEC_ID_MJPEG  //AV_CODEC_ID_RAWVIDEO
    if (!vcodec) {
        cerr << "can't find the codec id " << m_ic->streams[videoStream]->codecpar->codec_id << endl;
        return -1;
    }
    cerr << "find the AVCodec " << m_ic->streams[videoStream]->codecpar->codec_id << endl;

    m_vc = avcodec_alloc_context3(vcodec);

    ///配置解码器上下文参数
    avcodec_parameters_to_context(m_vc, m_ic->streams[videoStream]->codecpar);
    //n线程解码
    m_vc->thread_count = 1;

    ///打开解码器上下文
    re = avcodec_open2(m_vc, nullptr, nullptr);
    if (re != 0) {
        av_strerror(re, errorbuf, sizeof(errorbuf) - 1);
        cerr << "avcodec_open2 failed:" << errorbuf << endl;
        return -1;
    }
    isPlay = true;
    cerr << "avcodec_open2 success!" << endl;

    //alloc AVPacket并初始化
    m_pkt = av_packet_alloc();
    m_frame = av_frame_alloc();

    //像素格式和尺寸转换上下文
    m_vctx = nullptr;

    // Start thread to load frame
    loader = thread(&Camera::loaderThread, this);
    cerr << "success!" << endl;
    return 0;
}

bool Camera::initPkt() {
    if (!m_pkt) {
        m_pkt = av_packet_alloc();
        if(m_pkt)
        {
            memset(m_pkt, 0, sizeof(AVPacket));
        }else
        {
        }
    }

    if (!devio_lock.try_lock()) {
        cerr << "initPkt can't get device lock" << endl;
        av_packet_free(&m_pkt);
        return false;
    }

    if (!m_ic || !m_vc) {
        devio_lock.unlock();
        av_packet_free(&m_pkt);
        cerr << "(!m_ic || !m_vc)" <<endl;
        return false;
    }

    int err = av_read_frame(m_ic, m_pkt);
    if (err != 0 || !m_pkt || m_pkt->size <= 0 || !m_pkt->data) {
        av_strerror(err, errorbuf, 1024);
//        cerr << "read_frame error " << errorbuf << endl;
        devio_lock.unlock();
        av_packet_free(&m_pkt);
        return false;
    }

    int re = avcodec_send_packet(m_vc, m_pkt);
    devio_lock.unlock();
    //av_packet_unref(m_pkt);
    av_packet_free(&m_pkt);
    if (re != 0) {
        cerr << "int re = avcodec_send_packet(m_vc, m_pkt);"<<endl;
        return false;
    }
    return true;
}

bool Camera::readFrame() {
    if (!devio_lock.try_lock()) {
        cerr << "readFrame can't get device lock" << endl;
        return false;
    }

    if (!m_vc) {
        devio_lock.unlock();
        cerr << "if (!m_vc) {" << endl;
        return false;
    }
    if(!m_frame)
    {
       m_frame = av_frame_alloc();
    }
    memset(m_frame->verify_data, 0x00, 1024);
    m_frame->verify_data_length = 0;
    int re = avcodec_receive_frame(m_vc, m_frame);
    devio_lock.unlock();
    if (re != 0) {
//        cerr << "if (re != 0) {" << endl;
        //av_frame_unref(m_frame);
        av_frame_free(&m_frame);
        return false;
    }
    return true;
}

void Camera::parseFrame() {

    if (m_frame->linesize[0] <= 0) {
        cerr << "parseFrame: something null" << endl;
        //av_frame_unref(m_frame);
        return;
    }
//    cerr <<"parseFrame widthxheight"<<width<<"x"<<height<<endl;
//    cerr <<"m_frame->widthxm_frame->height"<<m_frame->width<<"x"<<m_frame->height<<endl;
    SetResolution(m_frame->width,m_frame->height);
    m_vctx = sws_getCachedContext(
            m_vctx,
            m_frame->width, m_frame->height,
            (AVPixelFormat) m_frame->format,//(AVPixelFormat)frame->format
            m_frame->width, m_frame->height,
            AV_PIX_FMT_BGRA,
            SWS_FAST_BILINEAR,
            nullptr, nullptr, nullptr);

    if (m_vctx) {
        if (frameImg == NULL) {
            frameImg = new uchar[width * height * 4];
            memset(frameImg, 0, width * height * 4);
            qDebug()<<"frameImg = new uchar[width * height * 4];";
        }


        frame_lock.lock();

        uint8_t *data[AV_NUM_DATA_POINTERS] = {0};
        data[0] = (uint8_t *) frameImg;
        int lines[AV_NUM_DATA_POINTERS] = {0};
        lines[0] = m_frame->width * 4;
        int re = sws_scale(m_vctx,
                           m_frame->data,
                           m_frame->linesize,
                           0,
                           m_frame->height,
                           data,
                           lines
        );
        sws_freeContext(m_vctx);
        m_vctx = nullptr;

        isNewFrame = true;

        if (m_frame->verify_data_length > 14) {
            isNewVerify = true;
            memset(verifyMsg, 0x00, sizeof(verifyMsg));
            memcpy(verifyMsg, m_frame->verify_data, m_frame->verify_data_length);
            char *endOfJson = strrchr(verifyMsg, '}');
           // *(endOfJson + 1) = '\0';//fanzhao why
            verifyMsgSize = endOfJson - verifyMsg + 1;

            //cerr << "verify data:" << verifyMsg << endl;
        }
        frame_lock.unlock();
    } else {
        cerr << "vctx is null" << endl;
    }

    //av_frame_unref(m_frame);
    av_frame_free(&m_frame);
}

void Camera::CloseCamera() {
    std::lock_guard<std::mutex> lock(devio_lock);
    isPlay = false;
    if (m_ic) {
        avformat_close_input(&m_ic);
    }
    if (m_vc) {
        avcodec_free_context(&m_vc);
    }
    if (m_frame) {
        av_frame_free(&m_frame);
    }
    if (m_vctx) {
        sws_freeContext(m_vctx);
        m_vctx = nullptr;
    }
    if (frameImg != nullptr) {
        delete[] frameImg;
        frameImg = nullptr;
    }

    isexit = true;
    if (loader.joinable())
        loader.join();
}

void Camera::loaderThread() {
    cerr << "loaderThread running at:" << std::this_thread::get_id() << endl;
    auto retry_time = chrono::microseconds(10);

    while (!isexit) {
        if (!isPlay) {
            break;
        }
        if (!initPkt()) {
            this_thread::sleep_for(retry_time);
            continue;
        }
        while (!isexit) {
            if (!readFrame())break;
            parseFrame();
        }
    }

    cerr << "loaderThread stop" << endl;
}

int Camera::GetFrame(char *imageBuf, int imageSize, char *verifyBuf, int verifySize) {
    std::lock_guard<std::mutex> lock(frame_lock);

    if (imageSize > 0) {
        if (imageSize != FrameSize()) {
            return 1;
        }

        if (!isNewFrame || frameImg == NULL) {
//            if(!isNewFrame)
//            {
//                cerr << "isNewFrame:false" << isNewFrame;
//            }else
//            {
//                cerr << "frameImg == NULL";
//            }

            return -1;
        }

        memcpy(imageBuf, frameImg, imageSize);
    }

    if (verifySize > 0) {
        if (verifySize < verifyMsgSize) {
            return 2;
        }

        if (isNewVerify) {
            memset(verifyBuf, 0, verifySize);
            memcpy(verifyBuf, verifyMsg, verifyMsgSize);
        }
    }

    isNewFrame = false;
    isNewVerify = false;

    return 0;
}
