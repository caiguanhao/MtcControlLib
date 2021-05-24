#pragma once

#include "stdafx.h"
#include "Serial.h"
#include "camera.h"

class Device : public Serial, public Camera {
public:
//    bool upload_cancel = false;
    int uploaded = 0;
    thread uploader;
    bool upload_cancel = false;

    int uploaded_image = 0;
    thread uploader_image;

    bool batch_mode = false;
    bool in_scope = false;
};

#define DEV_HANDLE Device*
