#include "mtc_interface.h"
#include<QThread>
int upload_slice(DEV_HANDLE dev, int seek, const char *data, int length)
{
    qDebug() << "-----------upload_slice(DEV_HANDLE dev, int seek, const char *data, int length) thread is " << QThread::currentThread();
    if (dev->upload_cancel) {
        return -1;
    }

    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;
    int result = -1;

    send_tlvs = mtc_core_create_tlvs(MTC_TAG_UPLOAD_UPGRADE_PACKAGE, 4 + 4 + length, nullptr);
    *(int *) (send_tlvs->value) = seek;
    *(int *) (send_tlvs->value + 4) = length;
    memcpy(send_tlvs->value + 8, data, length);

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != nullptr) {
        result = recv_tlvs->value[0];
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    dev->uploaded++;

    return result;
}

void upload(DEV_HANDLE dev, int seek, char *data, int length) {
    // this function will delete data after done.
qDebug() << "-----------upload(DEV_HANDLE dev, int seek, char *data, int length) thread is " << QThread::currentThread();
    dev->uploaded = 0;

    if (length <= MTC_UPGRADE_SEGMENT_LENGTH) {
        if (upload_slice(dev, seek, data, length) != 0) {
            dev->uploaded = -1;
        }
        delete[] data;
        return;
    }

    int slices = ceil(length / MTC_UPGRADE_SEGMENT_LENGTH);
    int result = 0;
    int offset;
    for (offset = 0; offset < (slices - 1) * MTC_UPGRADE_SEGMENT_LENGTH; offset += MTC_UPGRADE_SEGMENT_LENGTH) {
        cerr << "offset = " << offset;
        if (upload_slice(dev, offset, &data[offset], MTC_UPGRADE_SEGMENT_LENGTH) != 0) {
            dev->uploaded = -1;
            delete[] data;
            return;
        }
    }
    if (offset < length) {
        cerr << "last_slice = " << length - offset;
        if (upload_slice(dev, offset, &data[offset], length - offset) != 0) {
            dev->uploaded = -1;
            delete[] data;
            return;
        }
    }

    delete[] data;
    return;
}

int upload_image_slice(DEV_HANDLE dev, const char* imageId, uint imageIdLength, int seek, char* data,uint imageLength, uint sliceLength, int islastSlice)
{
    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;
    int result = -1;

    send_tlvs = mtc_core_create_tlvs(0x0007,1 + imageIdLength + 4 + 4 + 1 + sliceLength, nullptr);
    *(int *)(send_tlvs->value) = imageIdLength;
    memcpy(send_tlvs->value + 1, imageId, imageIdLength);
    *(int *)(send_tlvs->value +1 + imageIdLength) = imageLength;
    *(int *)(send_tlvs->value + 1 + imageIdLength + 4) = seek;
    *(int *)(send_tlvs->value + 1 + imageIdLength + 8) = islastSlice;
    memcpy(send_tlvs->value + 1 + imageIdLength + 9, data, sliceLength);

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != nullptr)
    {
        result = recv_tlvs->value[0];
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    dev->uploaded_image++;

    return result;
}

void upload_image(DEV_HANDLE dev, const char* imageId, uint imageIdLength, char* imageData,uint imageLength)
{
    dev->uploaded_image = 0;

    if (imageLength <= MTC_IMAGE_SEGMENT_LENGTH) {
        if (upload_image_slice(dev, imageId, imageIdLength, 0, imageData, imageLength, imageLength, 0x00) != 0) {
            dev->uploaded_image = -1;
        }
        delete[] imageData;
        return;
    }

    int slices = ceil(imageIdLength / MTC_IMAGE_SEGMENT_LENGTH);
    int result = 0;
    int offset;
    for (offset = 0; offset < (slices - 1)*MTC_IMAGE_SEGMENT_LENGTH; offset += MTC_IMAGE_SEGMENT_LENGTH) {
        if (upload_image_slice(dev, imageId, imageIdLength, offset, &imageData[offset],imageIdLength, MTC_IMAGE_SEGMENT_LENGTH, 0x00) != 0) {
            dev->uploaded_image = -1;
            delete[] imageData;
            return;
        }
    }
    if (offset < imageIdLength) {
        if (upload_image_slice(dev, imageId, imageIdLength, offset,&imageData[offset], imageLength, imageLength - offset,0x01) != 0) {
            dev->uploaded_image = -1;
            delete[] imageData;
            return;
        }
    }

    delete[] imageData;
    return;
}
