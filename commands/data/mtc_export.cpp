#include "mtc_export.h"
#include<QThread>

EXPORT int addImageByCamera(DEV_HANDLE dev, const char* imageId, uint imageIdLength) {
    mtc_image image;
    int result = -1;
    if (imageId == nullptr || imageIdLength <= 0) return 1;

    memset(image.id, 0x00, sizeof(image.id));
    memcpy(image.id, imageId, imageIdLength);
    image.id_length = imageIdLength;

    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;

    send_tlvs = mtc_core_create_tlvs(MTC_TAG_ADD_IMAGE, 1 + 4 + image.id_length, NULL);
    send_tlvs->value[0] = MTC_ADD_IMAGE_LOCAL_CAPTURE;
    *(int *)(send_tlvs->value + 1) = image.id_length;
    memcpy(send_tlvs->value + 1 + 4, image.id, image.id_length);

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != nullptr)
    {
        result = recv_tlvs->value[0];
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}
//AI控制命令{
//返回的camera抓取图片数据大概有20-80k
EXPORT int addImageByCameraImage(DEV_HANDLE dev, const char* imageId, uint imageIdLength, char* ret, uint ret_size) {
    mtc_image image;
    int result = -1;
    if (imageId == nullptr || imageIdLength <= 0) return 1;
    if (ret_size < 90 * 1024)
        return -2;
    memset(ret, 0x00, ret_size);
    memset(image.id, 0x00, sizeof(image.id));
    memcpy(image.id, imageId, imageIdLength);
    image.id_length = imageIdLength;

    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;

    send_tlvs = mtc_core_create_tlvs(MTC_TAG_ADD_IMAGE, 1 + 4 + image.id_length, NULL);
    send_tlvs->value[0] = MTC_ADD_IMAGE_LOCAL_CAPTURE_IMAGE;
    *(int *)(send_tlvs->value + 1) = image.id_length;
    memcpy(send_tlvs->value + 1 + 4, image.id, image.id_length);

    recv_tlvs = mtc_core_send_recv(send_tlvs, 10000, dev);
    if (recv_tlvs != nullptr)
    {
        unsigned int leng = recv_tlvs->length - 5;
        if (leng > ret_size)
        {
            result = -2;
        }
        else
        {
            result = recv_tlvs->value[0];
            memcpy(ret, recv_tlvs->value + 5, leng);
        }
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

EXPORT int addImageByPc(DEV_HANDLE dev, const char* imageId, uint imageIdLength, char* data, uint length) {
    mtc_image image;
    int result = -1;
    if (imageId == nullptr || imageIdLength <= 0)
        return -2;
    if (data == nullptr || length <= 0)
        return -3;


    memset(image.id, 0x00, sizeof(image.id));
    memcpy(image.id, imageId, imageIdLength);
    image.id_length = imageIdLength;

    if (data != nullptr && length != 0) {
        image.image = (unsigned char*)data;
        image.image_length = length;
    }

    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;

    send_tlvs = mtc_core_create_tlvs(MTC_TAG_ADD_IMAGE, 1 + 4 + image.id_length + 4 + image.image_length, NULL);
    send_tlvs->value[0] = MTC_ADD_IMAGE_REMOTE_INPUT;
    *(int *)(send_tlvs->value + 1) = image.id_length;
    memcpy(send_tlvs->value + 1 + 4, image.id, image.id_length);
    *(int *)(send_tlvs->value + 1 + 4 + image.id_length) = image.image_length;
    memcpy(send_tlvs->value + 1 + 4 + image.id_length + 4, image.image, image.image_length);

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != nullptr)
    {
        result = recv_tlvs->value[0];
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

//如果图片数据大于50K，需要分片
//EXPORT int uploadAddImageByPcSlice(DEV_HANDLE dev, const char* imageId, uint imageIdLength, char* ImageData, uint imageLength) {
//    char* tmpData = new char[imageLength];
//    memcpy(tmpData, ImageData, imageLength);

//    dev->uploader_image = thread(upload_image,dev, imageId, imageIdLength, tmpData, imageLength);
//    return ceil(imageLength / MTC_IMAGE_SEGMENT_LENGTH);
//}
//EXPORT int checkUploadImageState(DEV_HANDLE dev) {
//    return dev->uploaded_image;
//}

EXPORT int uploadAddImageSlice(DEV_HANDLE dev, const char* imageId, uint imageIdLength, char *sliceData, uint sliceDataLength,int position,uint imageTotalLength,int islastSlice) {
    if(sliceDataLength > MTC_UPGRADE_SEGMENT_LENGTH)
    {
        return -2;
    }

    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;
    int result = -1;

    send_tlvs = mtc_core_create_tlvs(0x0007,1 + imageIdLength + 4 + 4 + 1 + sliceDataLength, nullptr);
    *(int *)(send_tlvs->value) = imageIdLength;
    memcpy(send_tlvs->value + 1, imageId, imageIdLength);
    *(int *)(send_tlvs->value +1 + imageIdLength) = imageTotalLength;
    *(int *)(send_tlvs->value + 1 + imageIdLength + 4) = position;
    *(int *)(send_tlvs->value + 1 + imageIdLength + 8) = islastSlice;
    memcpy(send_tlvs->value + 1 + imageIdLength + 9, sliceData, sliceDataLength);

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

//导出ID列表 最大684K
EXPORT int getDeviceFaceID(DEV_HANDLE dev, char* ret, uint ret_buf_size)
{
    if (ret_buf_size < 684*1024)
    {
        return -2;
    }
    memset(ret, 0, ret_buf_size);
    int result = -1;
    int isTrue = -1;
    bool isCyclic = true;
    int totalRecIdLen = 0;
    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;

    send_tlvs = mtc_core_create_tlvs(0x0008, 4, NULL);
    int i = 0;
    int j = 0;
    while (isCyclic)
    {
        *(int *)send_tlvs->value = totalRecIdLen;
        recv_tlvs = mtc_core_send_recv_big(send_tlvs, 15000, dev);
        if (recv_tlvs != nullptr)
        {
            i++;
            int recvLen = recv_tlvs->length - 6;
            isTrue = recv_tlvs->value[0];
            if (0x00 == isTrue)
            {
                j++;
                qDebug()<<"0x00 == result j---"<<j;
                memcpy(ret+totalRecIdLen, recv_tlvs->value + 6, recvLen);
                totalRecIdLen += recvLen;
                if (recv_tlvs->value[1] == 0x01)
                {
                    qDebug()<<"recv_tlvs->value[1] == 0x01";
                    result = 0;
                    mtc_core_free_tlvs(recv_tlvs);
                    break;
                }
                mtc_core_free_tlvs(recv_tlvs);
            }
            else
            {
                qDebug()<<"result------"<<result;
                mtc_core_free_tlvs(recv_tlvs);
                break;
            }

        }
        else
        {
            qDebug()<<"recv_tlvs == nullptr";
            break;
        }
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}


EXPORT int deleteImage(DEV_HANDLE dev, int mode, const char* imageId, uint imageIdLength) {

    if (mode != MTC_DEL_IMAGE_ALL && (imageId == nullptr || imageIdLength <= 0)) return -2;

    int result = -1;
    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;

    if (mode == MTC_DEL_IMAGE_ALL)
    {
        send_tlvs = mtc_core_create_tlvs(MTC_TAG_DEL_IMAGE, 1, nullptr);
        send_tlvs->value[0] = MTC_DEL_IMAGE_ALL;
    }
    else
    {
        send_tlvs = mtc_core_create_tlvs(MTC_TAG_DEL_IMAGE, 1 + 4 + imageIdLength, nullptr);
        send_tlvs->value[0] = MTC_DEL_IMAGE_ONE;
        *(int *)(send_tlvs->value + 1) = imageIdLength;
        memcpy(send_tlvs->value + 1 + 4, imageId, imageIdLength);
    }
    recv_tlvs = mtc_core_send_recv_big(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != nullptr)
    {
        result = recv_tlvs->value[0];
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

EXPORT int queryId(DEV_HANDLE dev, int mode, const char* imageId, uint imageIdLength)
{

    if (mode != MTC_QUERY_ID_BY_ID && (imageId == nullptr || imageIdLength <= 0)) return -2;

    int result = -1;
    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;

    if (mode == MTC_QUERY_ID_BY_ID)
    {
        send_tlvs = mtc_core_create_tlvs(MTC_TAG_QUERY_ID, 1 + 4 + imageIdLength, NULL);
        send_tlvs->value[0] = MTC_QUERY_ID_BY_ID;
        *(int *)(send_tlvs->value + 1) = imageIdLength;
        memcpy(send_tlvs->value + 1 + 4, imageId, imageIdLength);
    }
    else
    {
        return -3;
    }
    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != nullptr)
    {
        result = recv_tlvs->value[0];
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

EXPORT int inputImage(DEV_HANDLE dev, int mode, char* data, uint length, char* ret, uint ret_size) {

    mtc_image image;
    int result = -1;


    if (data != nullptr && length != 0) {
        image.image = (unsigned char*)data;
        image.image_length = length;
    }

    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;

    if (mode == MTC_ADD_IMAGE_REMOTE_INPUT)
    {
        send_tlvs = mtc_core_create_tlvs(MTC_TAG_INPUT_IMAGE, 1 + 4 + image.image_length, NULL);
        send_tlvs->value[0] = MTC_ADD_IMAGE_REMOTE_INPUT;
        *(int *)(send_tlvs->value + 1) = image.image_length;
        memcpy(send_tlvs->value + 1 + 4, image.image, image.image_length);
    }
    else if(mode == 0x00)//add image by module
    {
        send_tlvs = mtc_core_create_tlvs(MTC_TAG_INPUT_IMAGE, 1 , NULL);
        send_tlvs->value[0] = 0x00;
    }
    else
    {
        return -1;
    }

    recv_tlvs = mtc_core_send_recv(send_tlvs, 6000, dev);
    if (recv_tlvs != nullptr)
    {
        result = recv_tlvs->value[0];
        if (result == 0x00)
        {
            int recv_tlvs_vslue_len = *(int *)(recv_tlvs->value+1);
            if (ret_size < recv_tlvs_vslue_len)
            {
                //memcpy(ret, recv_tlvs->value+5, ret_size);
                result = -2;
            }
            else
            {
                memcpy(ret, recv_tlvs->value+5, recv_tlvs_vslue_len);
                result = 0;
            }
        }
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

EXPORT int addFeature(DEV_HANDLE dev, const char* featureId, uint featureIdLength, char* data, uint length)
{
    mtc_feature feature;
    int result = -1;
    if (featureId == nullptr || featureIdLength <= 0) return -2;

    memset(feature.id, 0x00, sizeof(feature.id));
    memcpy(feature.id, featureId, featureIdLength);
    feature.id_length = featureIdLength;

    if (data != nullptr && length != 0) {
        feature.feature = (unsigned char*)data;
        feature.feature_length = length;
    }
    else
    {
        return -3;
    }

    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;

    send_tlvs = mtc_core_create_tlvs(MTC_TAG_ADD_FEATURE, 4 + feature.id_length + 4 + feature.feature_length, NULL);
    *(int *)(send_tlvs->value) = feature.id_length;
    memcpy(send_tlvs->value + 4, feature.id, feature.id_length);
    *(int *)(send_tlvs->value + 4 + feature.id_length) = feature.feature_length;
    memcpy(send_tlvs->value + 4 + feature.id_length + 4, feature.feature, feature.feature_length);

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != nullptr)
    {
        result = recv_tlvs->value[0];
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

EXPORT int queryFeature(DEV_HANDLE dev, const char* featureId, uint featureIdLength, char* ret, uint ret_size)
{
    //特征值一般为1.5M大小，建议ret_size设置2048大小
    if ((featureId == nullptr || featureIdLength <= 0)) return -2;

    int result = -1;
    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;

    send_tlvs = mtc_core_create_tlvs(MTC_TAG_QUERY_FEATURE, 4 + featureIdLength, NULL);
    *(int *)(send_tlvs->value) = featureIdLength;
    memcpy(send_tlvs->value + 4, featureId, featureIdLength);

    recv_tlvs = mtc_core_send_recv_big(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != NULL)
    {
        if (strlen((const char*)recv_tlvs->value) > ret_size)
        {
            mtc_core_free_tlvs(recv_tlvs);
            mtc_core_free_tlvs(send_tlvs);
            return -3;
        }
        result = recv_tlvs->value[0];
        if (result == 0x00)
        {
            uint featureLen = *(int *)(recv_tlvs->value + 1 + 4 + featureIdLength);
            if (featureLen != 0)
            {
                memcpy(ret, recv_tlvs->value + 1 + 4 + featureIdLength + 4, featureLen);
            }
            else
            {
                //result
            }
        }
        if (result == 0x01)
        {
            //UINT idLen = *(recv_tlvs->value + 1);
            //UINT featureLen = *(recv_tlvs->value + 2 + idLen);
            memcpy(ret, 0x00, 1);
        }
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

EXPORT int startOnetoNumRecognize(DEV_HANDLE dev, int reomode, int mulmode)
{
    if (reomode != 0 && reomode != 1)
    {
        return -2;
    }
    if (mulmode != 0 && mulmode != 1)
    {
        return -3;
    }
    int result = -1;

    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;

    send_tlvs = mtc_core_create_tlvs(MTC_TAG_START_ONETONUM_RECOGNIZE, 1 + 1, NULL);
    send_tlvs->value[0] = reomode;
    send_tlvs->value[1] = mulmode;


    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != NULL)
    {
        result = recv_tlvs->value[0];
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

EXPORT int startOnetoOneRecognize(DEV_HANDLE dev, int reomode, int mulmode, char* data, uint length)
{
    if (reomode != 0 && reomode != 1)
    {
        return -2;
    }
    if (mulmode != 0 && mulmode != 1)
    {
        return -3;
    }
    mtc_image image;
    int result = -1;

    if (data != nullptr && length != 0) {
        image.image = (unsigned char*)data;
        image.image_length = length;
    }
    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;

    send_tlvs = mtc_core_create_tlvs(MTC_TAG_START_ONETOONE_RECOGNIZE, 1 + 1 + 4 + image.image_length, NULL);
    send_tlvs->value[0] = reomode;
    send_tlvs->value[1] = mulmode;
    *(int *)(send_tlvs->value + 1 + 1) = image.image_length;
    memcpy(send_tlvs->value + 1 + 1 + 4, image.image, image.image_length);


    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != NULL)
    {
        result = recv_tlvs->value[0];
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

EXPORT int resumeRecognize(DEV_HANDLE dev)
{
    MTC_TLVS *send_tlvs = NULL;
    MTC_TLVS *recv_tlvs = NULL;
    int result = -1;

    send_tlvs = mtc_core_create_tlvs(MTC_TAG_RESUME_RECOGNIZE, 0, NULL);

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != NULL)
    {
        result = recv_tlvs->value[0];
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

EXPORT int pauseRecognize(DEV_HANDLE dev)
{
    MTC_TLVS *send_tlvs = NULL;
    MTC_TLVS *recv_tlvs = NULL;
    int result = -1;

    send_tlvs = mtc_core_create_tlvs(MTC_TAG_PAUSE_RECOGNIZE, 1, NULL);

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != NULL)
    {
        result = recv_tlvs->value[0];
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

EXPORT int queryRecognize(DEV_HANDLE dev)
{
    MTC_TLVS *send_tlvs = NULL;
    MTC_TLVS *recv_tlvs = NULL;
    int result = -1;

    send_tlvs = mtc_core_create_tlvs(MTC_TAG_QUERY_RECOGNIZE, 1, NULL);
    send_tlvs->value[0] = 0x00;

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != NULL)
    {
        result = recv_tlvs->value[0];
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

//mode 0x00: 算法配置 0x0x: 待续
EXPORT int setReoconfig(DEV_HANDLE dev, int mode, char* jsonData, uint jsonDataLength)
{
    mtc_json json;
    if (jsonData != nullptr && jsonDataLength != 0) {
        json.json = (unsigned char*)jsonData;
        json.json_length = jsonDataLength;
    }
    else
    {
        return -3;
    }

    MTC_TLVS *send_tlvs = NULL;
    MTC_TLVS *recv_tlvs = NULL;
    int result = -1;

    send_tlvs = mtc_core_create_tlvs(MTC_TAG_SET_REOCONFIG, 2 + json.json_length, NULL);
    send_tlvs->value[0] = 0x00;//0x00:下发配置 0x01:查询配置
    send_tlvs->value[1] = mode;//0x00:算法配置
    memcpy(send_tlvs->value + 2, json.json, json.json_length);

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);

    if (recv_tlvs != NULL)
    {
        result = recv_tlvs->value[0];
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;

}

//mode 0x00: 算法配置 0x0x: 待续
EXPORT int getReoconfig(DEV_HANDLE dev, int mode, char* ret, uint ret_buf_size)//20200311写下，下次构件支持更新
{
    MTC_TLVS *send_tlvs = NULL;
    MTC_TLVS *recv_tlvs = NULL;
    int result = -1;

    send_tlvs = mtc_core_create_tlvs(MTC_TAG_SET_REOCONFIG, 2, NULL);
    send_tlvs->value[0] = 0x01;//0x00:下发配置 0x01:查询配置
    send_tlvs->value[1] = mode;//0x00:算法配置
    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);

    if (recv_tlvs != NULL)
    {
        int retLen = recv_tlvs->length;
        if (retLen > ret_buf_size)
        {
            memcpy(ret, recv_tlvs->value+1, ret_buf_size);
            result = -2;
        }
        else if (retLen <= ret_buf_size)
        {
            memcpy(ret, recv_tlvs->value+1, retLen);
            result = 0;
        }
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

EXPORT int openAutoUploadAiInfo(DEV_HANDLE dev)//预留待完善
{
    MTC_TLVS *send_tlvs = NULL;
    MTC_TLVS *recv_tlvs = NULL;
    int result = -1;

    send_tlvs = mtc_core_create_tlvs(0x0018, 1, NULL);
    send_tlvs->value[0] = 0x00;

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != NULL)
    {
        result = recv_tlvs->value[0];
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

EXPORT int closeAutoUploadAiInfo(DEV_HANDLE dev)//预留待完善
{
    MTC_TLVS *send_tlvs = NULL;
    MTC_TLVS *recv_tlvs = NULL;
    int result = -1;

    send_tlvs = mtc_core_create_tlvs(0x0018, 1, NULL);
    send_tlvs->value[0] = 0x01;
    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != NULL)
    {
        result = recv_tlvs->value[0];
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

EXPORT int openAutoUploadFaceInfoInFrame(DEV_HANDLE dev)
{
    MTC_TLVS *send_tlvs = NULL;
    MTC_TLVS *recv_tlvs = NULL;
    int result = -1;

    send_tlvs = mtc_core_create_tlvs(0x0019, 1, NULL);
    send_tlvs->value[0] = 0x00;

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != NULL)
    {
        result = recv_tlvs->value[0];
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

EXPORT int closeAutoUploadFaceInfoInFrame(DEV_HANDLE dev)
{
    MTC_TLVS *send_tlvs = NULL;
    MTC_TLVS *recv_tlvs = NULL;
    int result = -1;

    send_tlvs = mtc_core_create_tlvs(0x0019, 1, NULL);
    send_tlvs->value[0] = 0x01;

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != NULL)
    {
        result = recv_tlvs->value[0];
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

EXPORT int getDeviceFaceLibraryNum(DEV_HANDLE dev, char* ret, uint ret_buf_size)
{
    if (ret_buf_size < 4)
    {
        return -2;
    }
    int result = -1;
    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;

    send_tlvs = mtc_core_create_tlvs(0x0035, 0, NULL);

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != nullptr)
    {
        if (ret_buf_size < 1)
        {
            result = -1;
        }
        else
        {
            result = recv_tlvs->value[0];
            if (0x00 == result)
            {
                memcpy(ret, recv_tlvs->value + 1, 4);
            }
        }
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}
//}AI控制命令 后续添加主动上报处理部分


//系统控制命令{
EXPORT int ping(DEV_HANDLE dev, const char* data, int length) {

    if (length == 0) {
        char preset[32];
        memset(preset, 0x91, 32);
        data = preset;
        length = 32;
    }

    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;
    int result = -1;

    send_tlvs = mtc_core_create_tlvs(MTC_TAG_PING_TEST, length, (unsigned char*)data);

    if (send_tlvs != nullptr)
    {
        recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    }
    if (recv_tlvs != nullptr)
    {
        if (recv_tlvs->length == length && memcmp(data, recv_tlvs->value, length) == 0)
        {
            result = 0;
        }
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result == 0 ? 0 : -1;
}

EXPORT int uploadPackageSlice(DEV_HANDLE dev, int position, char *data, uint length) {
    if(length > MTC_UPGRADE_SEGMENT_LENGTH)
    {
        return -2;
    }
    unsigned char* uploadSlice = (unsigned char*)data;

    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;
    int result = -1;

    send_tlvs = mtc_core_create_tlvs(MTC_TAG_UPLOAD_UPGRADE_PACKAGE, 4 + 4 + length, nullptr);
    *(int *)(send_tlvs->value) = position;
    *(int *)(send_tlvs->value + 4) = length;
    memcpy(send_tlvs->value + 8, uploadSlice, length);

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if(recv_tlvs != nullptr)
    {
        result = recv_tlvs->value[0];
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

EXPORT int applyUpgrade(DEV_HANDLE dev, const char* hash, int length) {
    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;
    int result = -1;

    send_tlvs = mtc_core_create_tlvs(MTC_TAG_APPLY_UPGRADE_PACKAGE, 4 + length, nullptr);
    *(int *)((char *)send_tlvs->value) = length;
    memcpy(send_tlvs->value + 4, hash, length);

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != nullptr)
    {
        result = recv_tlvs->value[0];
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

//EXPORT int uploadPackage(DEV_HANDLE dev, int seek, const char* data, uint length) {
//    dev->upload_cancel = false;
//    dev->uploaded = 0;

//    char* tmpData = new char[length];
//    memcpy(tmpData, data, length);
//    qDebug()<<"uploadPackage";
//    qDebug() << "-----------uploadPackage(DEV_HANDLE dev, int seek, const char* data, uint length) thread is " << QThread::currentThread();
//    dev->uploader = thread(upload, dev, seek, tmpData, length);
//    return ceil(length / MTC_UPGRADE_SEGMENT_LENGTH);
//}

//EXPORT void cancelUpload(DEV_HANDLE dev) {
//    dev->upload_cancel = true;
//    if (dev->uploader.joinable()) {
//        dev->uploader.join();
//    }
//}

//EXPORT int checkUploadState(DEV_HANDLE dev) {
//    return dev->uploaded;
//}

EXPORT int getSysVer(DEV_HANDLE dev, char* ret, uint ret_buf_size) {
    int result = -1;

    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;

    send_tlvs = mtc_core_create_tlvs(0x1003, 0, NULL);

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != nullptr)
    {
        int retLen = recv_tlvs->length;
        if (retLen > ret_buf_size)
        {
            memcpy(ret, recv_tlvs->value, ret_buf_size);
            result = -2;
        }
        else if (retLen <= ret_buf_size)
        {
            memcpy(ret, recv_tlvs->value, retLen);
            result = 0;
        }
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

//0x00: 直接重启
//0x01: 切U盘用户模式重启
//0x02: 非U盘用户模式重启
//0x03: 切U盘工厂模式
//0x04: 非U盘工厂模式
EXPORT int reboot(DEV_HANDLE dev, int mode)
{
    int result = -1;

    if (mode > 4 || mode < 0) {
        return result;
    }

    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;

    send_tlvs = mtc_core_create_tlvs(0x1004, 1, NULL);
    send_tlvs->value[0] = mode;

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != nullptr)
    {
        result = recv_tlvs->value[0];
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

//模式切换接口
//1、提供用户模式与工程模式切换的接口
//2、应用的界面上做一个切换工程模式的接口
//注意设置该命令以后必须重启才能生效，reboot
EXPORT int setUmodeToEngin(DEV_HANDLE dev)
{
    int result = -1;
    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;

    send_tlvs = mtc_core_create_tlvs(0x1004, 1, NULL);
    send_tlvs->value[0] = 0x04;

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != nullptr)
    {
        result = recv_tlvs->value[0];
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

EXPORT int reset(DEV_HANDLE dev) {

    int result = -1;

    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;

    send_tlvs = mtc_core_create_tlvs(0x1004, 1, NULL);
    send_tlvs->value[0] = 0x00;

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != nullptr)
    {
        result = recv_tlvs->value[0];
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

//0x00: 清空用户配置
//0x01: 保存用户配置
EXPORT int recovery(DEV_HANDLE dev, int mode) {
    if (mode != 0x00 && mode != 0x01)
    {
        return -2;
    }

    int result = -1;
    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;

    send_tlvs = mtc_core_create_tlvs(0x1005, 1, NULL);
    send_tlvs->value[0] = mode;

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != nullptr)
    {
        result = recv_tlvs->value[0];
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

//mode SN类型  0x00：PCB  0x01：Device
EXPORT int getDevSn(DEV_HANDLE dev, int mode, char* ret, uint ret_buf_size) {
    if (mode != 0x00 && mode != 0x01)
    {
        return -2;
    }
    int result = -1;

    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;

    send_tlvs = mtc_core_create_tlvs(0x2001, 1, NULL);
    send_tlvs->value[0] = mode;

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != nullptr)
    {
        int retLen = recv_tlvs->length - 4;
        if (retLen == 0)
        {
            result = -1;
        }
        else if (retLen > ret_buf_size)
        {
            memcpy(ret, recv_tlvs->value + 4, ret_buf_size);
            result = -2;
        }
        else if (retLen <= ret_buf_size)
        {
            memcpy(ret, recv_tlvs->value + 4, retLen);
            result = 0;
        }
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

EXPORT int getDevModel(DEV_HANDLE dev, char* ret, uint ret_buf_size)
{
    int result = -1;

    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;

    send_tlvs = mtc_core_create_tlvs(0x2002, 0, NULL);

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != nullptr)
    {
        int retLen = recv_tlvs->length - 4;
        if (retLen > ret_buf_size)
        {
            memcpy(ret, recv_tlvs->value + 4, ret_buf_size);
            result = -2;
        }
        else if (retLen <= ret_buf_size)
        {
            memcpy(ret, recv_tlvs->value + 4, retLen);
            result = 0;
        }
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);
    return result;
}


//0x00: 开启推流
//0x01: 关闭推流
EXPORT int setUvcSwitch(DEV_HANDLE dev, int mode)//暂定，后续补充
{
    if (mode != 0x00 && mode != 0x01)
    {
        return -2;
    }

    int result = -1;

    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;

    send_tlvs = mtc_core_create_tlvs(0x2003, 1, NULL);
    send_tlvs->value[0] = mode;

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != nullptr)
    {
        result = recv_tlvs->value[0];
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}


//0x00：打开摄像头
//0x01：关闭摄像头
//0x02：获取摄像头状态//单独为获取函数处理

//0x00：RGB摄像头
//0x01：IR摄像头
//摄像头开关
EXPORT int setCameraStream(DEV_HANDLE dev, int disposeMode, int cameraMode)//暂定，后续补充
{
    if (disposeMode < 0x00 || disposeMode > 0x01)
    {
        return -2;
    }
    if (cameraMode != 0x00 && cameraMode != 0x01)
    {
        return -3;
    }
    int result = -1;
    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;

    send_tlvs = mtc_core_create_tlvs(0x2004, 2, NULL);
    send_tlvs->value[0] = disposeMode;
    send_tlvs->value[1] = cameraMode;

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != nullptr)
    {
        result = recv_tlvs->value[0];
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}


//0x02：获取摄像头状态//单独为获取函数处理

//0x00：RGB摄像头
//0x01：IR摄像头
//摄像头开关
EXPORT int getCameraStream(DEV_HANDLE dev, int cameraMode, char* ret, uint ret_buf_size)//暂定，后续补充
{
    if (cameraMode != 0x00 && cameraMode != 0x01)
    {
        return -2;
    }
    int result = -1;
    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;

    send_tlvs = mtc_core_create_tlvs(0x2004, 2, NULL);
    send_tlvs->value[0] = 0x02;
    send_tlvs->value[1] = cameraMode;

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != nullptr)
    {
        result = recv_tlvs->value[0];
        int retLen = recv_tlvs->length - 1;//retLen 固定值为1
        if (result == 0x00)
        {
            if (retLen > ret_buf_size)
            {
                memcpy(ret, recv_tlvs->value + 1, ret_buf_size);
                result = -2;
            }
            else if (retLen <= ret_buf_size)
            {
                memcpy(ret, recv_tlvs->value + 1, retLen);
            }
        }
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}


//0x00：切换RGB
//0x01：切换IR
EXPORT int switchCamRgbIr(DEV_HANDLE dev, int mode) {
    if (mode < 0x00 || mode > 0x05)
    {
        return -2;
    }

    int result = -1;

    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;

    send_tlvs = mtc_core_create_tlvs(0x2005, 1, NULL);
    send_tlvs->value[0] = mode;

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != nullptr)
    {
        result = recv_tlvs->value[0];
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

//IR灯亮度设置接口,推荐0-200之间
EXPORT int setIRlight(DEV_HANDLE dev, int luminance)
{
    int result = -1;
    if (luminance > 200 || luminance < 0)
        return -2;
    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;
    send_tlvs = mtc_core_create_tlvs(0x2006, 2, NULL);
    send_tlvs->value[0] = 0x00;
    send_tlvs->value[1] = luminance;

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != nullptr)
    {
        result = recv_tlvs->value[0];
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

//0x00：打开IR灯
//0x01：关闭IR灯
//0x02：获取IR灯亮度
//0X03：设置IR灯亮度
EXPORT int getIRlight(DEV_HANDLE dev, char* ret, uint ret_buf_size)
{
    int result = -1;
    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;
    send_tlvs = mtc_core_create_tlvs(0x2006, 1, NULL);
    send_tlvs->value[0] = 0x01;

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != nullptr)
    {
        result = recv_tlvs->value[0];
        int retLen = recv_tlvs->length;//retLen 固定值为2
        if (result == 0x00)
        {
            if (ret_buf_size < 1)
            {
                result = -2;
            }
            else if (1 <= ret_buf_size)
            {
                memcpy(ret, recv_tlvs->value + 1, 1);

            }
        }
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}


EXPORT int setFrameRate(DEV_HANDLE dev, int frame_rate)
{
    int result = -1;
    if (frame_rate > 25 || frame_rate < 10)
        return result;
    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;
    send_tlvs = mtc_core_create_tlvs(0x2007, 2, NULL);
    send_tlvs->value[0] = 0x01;
    send_tlvs->value[1] = frame_rate;

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != nullptr)
    {
        result = recv_tlvs->value[0];
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

EXPORT int getFrameRate(DEV_HANDLE dev, char* ret, uint ret_buf_size)
{
    int result = -1;
    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;
    send_tlvs = mtc_core_create_tlvs(0x2007, 2, NULL);
    send_tlvs->value[0] = 0x00;

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != nullptr)
    {
        result = recv_tlvs->value[0];
        if (result == 0x00)
        {
            if (ret_buf_size < 1)
            {
                result = -1;
            }
            else
            {
                memcpy(ret, recv_tlvs->value + 1, 4);
            }
        }
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

// 1、支持屏的设置模式，横屏，竖屏
//2、可提供720P，360P的接口设置
//备注： M10：设备竖的放置时，只支持横屏显示；设备横放时，只支持竖屏显示
//M20：设备只支持横放（可旋转0 / 180度），支持竖屏
EXPORT int setResolution(DEV_HANDLE dev, int mode)
{
    int result = -1;
    if (mode != 720 && mode != 360)
    {
        return result;
    }
    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;

    send_tlvs = mtc_core_create_tlvs(0x2008, 1, NULL);
    if (720 == mode)
    {
        send_tlvs->value[0] = 0x00;
    }
    else if (360 == mode)
    {
        send_tlvs->value[0] = 0x01;
    }

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != nullptr)
    {
        result = recv_tlvs->value[0];
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

//设置屏幕模式
//0x00：横屏模式
//0x01: 竖屏模式
EXPORT int setScreenDirection(DEV_HANDLE dev, int mode)
{
    int result = -1;
    if (mode != 1 && mode != 2 && mode != 3 && mode != 4)
        return result;
    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;
    send_tlvs = mtc_core_create_tlvs(0x2009, 2, NULL);
    send_tlvs->value[0] = 0x01;
    send_tlvs->value[1] = mode;

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != nullptr)
    {
        result = recv_tlvs->value[0];
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

EXPORT int getScreenDirection(DEV_HANDLE dev, char* ret, uint ret_buf_size)
{
    int result = -1;

    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;
    send_tlvs = mtc_core_create_tlvs(0x2009, 1, NULL);
    send_tlvs->value[0] = 0x00;

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != nullptr)
    {
        result = recv_tlvs->value[0];
        if (result == 0x00)
        {
            if (ret_buf_size < 1)
            {
                result = -2;
            }
            else
            {
                memcpy(ret, recv_tlvs->value + 1, 1);
            }
        }
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

EXPORT int setRotateAngle(DEV_HANDLE dev, int mode)
{
    int result = -1;
    if (mode != 0 && mode < 90 && mode != 180 && mode != 270)
        return result;
    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;
    send_tlvs = mtc_core_create_tlvs(0x200a, 1, NULL);
    if (0 == mode)
    {
        send_tlvs->value[0] = 0x00;
    }
    else if (90 == mode)
    {
        send_tlvs->value[0] = 0x01;
    }
    else if (180 == mode)
    {
        send_tlvs->value[0] = 0x02;
    }
    else if (270 == mode)
    {
        send_tlvs->value[0] = 0x03;
    }

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != nullptr)
    {
        result = recv_tlvs->value[0];
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

//设置50/60HZ频率抗闪烁：
//0x00:获取抗闪设置参数
//0x01:设置抗闪参数
//0x00:id为0
//0x01:id为1
//0x00:50HZ
//0x01:60HZ
//0x00:去使能
//0x01:使能
//需要特别注明：如果ai_config中的ae_mode字段值为0时，此处设置才生效，
//如果ae_mode字段值为1，则需要配置ai_config中的ae_flicker_freq。
EXPORT int setDeviceNoFlickerHz(DEV_HANDLE dev, int camera_id, int Hz, int enabled)
{
    int result = -1;
    if (camera_id != 0 && camera_id != 1)
    {
        return result;
    }
    if (Hz != 0x00 && Hz != 0x01)
    {
        return result;
    }
    if (enabled != 0 && enabled != 1)
    {
        return result;
    }
    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;

    send_tlvs = mtc_core_create_tlvs(0x200b, 4, NULL);
    send_tlvs->value[0] = 0x01;
    send_tlvs->value[1] = camera_id;
    send_tlvs->value[2] = Hz;
    send_tlvs->value[3] = enabled;

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != nullptr)
    {
        result = recv_tlvs->value[0];
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

EXPORT int getDeviceNoFlickerHz(DEV_HANDLE dev, int camera_id, char* ret, uint ret_buf_size)
{
    int result = -1;
    if (camera_id != 0 && camera_id != 1)
    {
        return result;
    }
    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;

    send_tlvs = mtc_core_create_tlvs(0x200b, 2, NULL);
    send_tlvs->value[0] = 0x00;
    send_tlvs->value[1] = camera_id;

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != nullptr)
    {
        result = recv_tlvs->value[0];
        if (result == 0x00)
        {
            if (ret_buf_size < 2)
            {
                result = -1;
            }
            else
            {
                memcpy(ret, recv_tlvs->value + 1, 2);
            }
        }
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

//获取当前光敏值(当前仅M20)
EXPORT int getLuminousSensitivityThreshold(DEV_HANDLE dev, char* ret, uint ret_buf_size)
{
    int result = -1;

    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;

    send_tlvs = mtc_core_create_tlvs(0x200c, 0, NULL);

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != nullptr)
    {
        result = recv_tlvs->value[0];
        if (result == 0x00)
        {
            if (ret_buf_size < 4)
            {
                result = -1;
            }
            else
            {
                memcpy(ret, recv_tlvs->value + 1, 4);
            }
        }
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}


EXPORT int getModuleAppVersion(DEV_HANDLE dev, char* ret, uint ret_buf_size)
{
    int result = -1;

    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;

    send_tlvs = mtc_core_create_tlvs(0x200d, 0, NULL);

    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != nullptr)
    {
        int versionLen = recv_tlvs->value[0];
        if (versionLen != 0x00)
        {
            if (ret_buf_size < versionLen)
            {
                result = -1;
            }
            else
            {
                memcpy(ret, recv_tlvs->value + 1, versionLen);
                result = 0;
            }
        }
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}

//设置流格式
//0x00:MJPEG
//0x01:H.264
//必须与FFMPEG格式统一，重新调用一遍connectCamera函数
EXPORT int setStreamFormat(DEV_HANDLE dev, int mode)
{
    int result = -1;
    if (mode != 0x00 || mode != 0x01)
    {
        return -2;
    }

    MTC_TLVS *send_tlvs = nullptr;
    MTC_TLVS *recv_tlvs = nullptr;

    send_tlvs = mtc_core_create_tlvs(0x200e, 1, NULL);
    send_tlvs->value[0] = mode;
    recv_tlvs = mtc_core_send_recv(send_tlvs, WAIT_TIME_OUT, dev);
    if (recv_tlvs != nullptr)
    {
        result = recv_tlvs->value[0];
        mtc_core_free_tlvs(recv_tlvs);
    }
    mtc_core_free_tlvs(send_tlvs);

    return result;
}
