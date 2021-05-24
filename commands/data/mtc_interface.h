#pragma once

#include "../../stdafx.h"
#include "../../device.h"

#include "mtc_core.h"
#include "mtc_package.h"

#define MTC_TAG_ADD_IMAGE		0x0001
#define MTC_TAG_DEL_IMAGE		0x0002
#define MTC_TAG_QUERY_ID		0x0003
#define MTC_TAG_INPUT_IMAGE     0x0004
#define MTC_TAG_ADD_FEATURE     0x0005
#define MTC_TAG_QUERY_FEATURE   0x0006

#define MTC_TAG_START_RECOGNIZE				0x0010
#define MTC_TAG_GET_RECOGNIZE_RECORD		0x0011
#define MTC_TAG_STOP_RECOGNIZE				0x0012
#define MTC_TAG_STATE_RECOGNIZE				0x0014

#define MTC_TAG_START_ONETONUM_RECOGNIZE	0x0010
#define MTC_TAG_START_ONETOONE_RECOGNIZE	0x0013
#define MTC_TAG_RESUME_RECOGNIZE			0x0014
#define MTC_TAG_PAUSE_RECOGNIZE				0x0015
#define MTC_TAG_QUERY_RECOGNIZE				0x0016
#define MTC_TAG_SET_REOCONFIG				0x0017
#define MTC_TAG_UPLOAD_UPGRADE_PACKAGE		0x0020
#define MTC_TAG_APPLY_UPGRADE_PACKAGE		0x0021

#define MTC_TAG_RECOGNIZE_UPLOAD			0x0030
#define MTC_TAG_ONETONUM_UPLOAD				0x0031
#define MTC_TAG_ONETOONE_UPLOAD				0x0032
#define MTC_TAG_IMAGE_UPLOAD				0x0033
#define MTC_TAG_FEATURE_UPLOAD				0x0034

#define MTC_TAG_PING_TEST					0x1000
#define MTC_TAG_SHELL_CMD					0x1001
#define MTC_TAG_COMMAND_MODE				0x1002

#define MTC_ADD_IMAGE_LOCAL_CAPTURE		0x00
#define MTC_ADD_IMAGE_REMOTE_INPUT		0x01
#define MTC_ADD_IMAGE_LOCAL_CAPTURE_IMAGE		0x02

#define MTC_SINGLE_MODE					0x00
#define MTC_MULTI_MODE					0x01
#define MTC_NOLIVING_MODE				0x00
#define MTC_LIVING_MODE					0x01

#define MTC_DEL_IMAGE_ALL				0x00
#define MTC_DEL_IMAGE_ONE				0x01
#define MTC_QUERY_ID_BY_ID			    0x00
#define MTC_QUERY_IMAGE_BY_IMAGE		0x01
#define MTC_FACE_RECOGNIZE_CONINUE		0x00
#define MTC_FACE_RECOGNIZE_SINGLE		0x01

#define MTC_TAG_OPEN_AUTO_UPLOAD_AI_INFO	0x0022
#define MTC_TAG_CLOSE_AUTO_UPLOAD_AI_INFO	0x0023
#define MTC_TAG_OPEN_FRAME_ADD_AI_INFO		0x0024
#define MTC_TAG_CLOSE_FRAME_ADD_AI_INFO		0x0025



#define MTC_IMAGE_ID_LENGTH			    32
#define MTC_FEATURE_ID_LENGTH			32
#define MTC_UPGRADE_SEGMENT_LENGTH		8080
#define MTC_IMAGE_SEGMENT_LENGTH		8080

typedef struct mtc_image
{
	unsigned int image_length;
	unsigned char *image;
	unsigned int id_length;
	unsigned char id[MTC_IMAGE_ID_LENGTH];
}mtc_image;

typedef struct mtc_feature
{
    unsigned int feature_length;
    unsigned char *feature;
    unsigned int id_length;
    unsigned char id[MTC_FEATURE_ID_LENGTH];
}mtc_feature;

typedef struct mtc_json
{
    unsigned int json_length;
    unsigned char *json;
}mtc_json;

#define WAIT_TIME_OUT 5000
//#define WAIT_TIME_OUT 15000

void upload(DEV_HANDLE dev, int seek, char* data, int length);
void upload_image(DEV_HANDLE dev, const char* imageId, uint imageIdLength, char* imageData, uint imageLength);
