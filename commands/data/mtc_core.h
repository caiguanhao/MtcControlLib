#pragma once

#include "device.h"

#define MTC_MAX_MESSAGE_LENGTH    1024

#pragma pack (push, 1)

typedef struct MTC_TLVS {
    struct MTC_TLVS *next;
    unsigned short tag;
    unsigned int length;
    unsigned char value[1];
} MTC_TLVS;

#pragma pack(pop)


typedef void(*MTC_CALLBACK)(unsigned short command, unsigned int length, unsigned char *data);


MTC_TLVS *mtc_core_create_tlvs(unsigned short command, unsigned int length, unsigned char *data);

int mtc_core_free_tlvs(MTC_TLVS *tlvs);

MTC_TLVS *mtc_core_parse_tlvs(unsigned char *data);

MTC_TLVS *mtc_core_send_recv(MTC_TLVS *tlvs, int msecond, DEV_HANDLE dev);

int mtc_core_send_tlvs(MTC_TLVS *tlvs, DEV_HANDLE dev);

MTC_TLVS *mtc_core_send_recv(MTC_TLVS *tlvs, int msecond, DEV_HANDLE dev);

MTC_TLVS *mtc_core_send_recv_big(MTC_TLVS *tlvs, int msecond, DEV_HANDLE dev);


