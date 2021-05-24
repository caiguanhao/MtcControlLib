/************************************************************************
Copyright  : 2018-2028,Shenzhen SenseTime Co.,Ltd
File name  : mtc_core.cpp
Description: the core process of mtc.
Author     : pengenhou
Date       : 2019/06/12
Version    : V1.0.0
History    : 2019/06/12, Create it by pengenhou
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <QThread>

#include "mtc_package.h"
#include "mtc_core.h"
#include "mtc_interface.h"


/******************************************************************************
Function   : mtc_core_create_tlvs
Description: create tlvs
Input      : the tlv data
Output     : void
Return     : the tlvs list.
******************************************************************************/
MTC_TLVS *mtc_core_create_tlvs(unsigned short command, unsigned int length, unsigned char *data)
{
	MTC_TLVS *tlvs = NULL;

	tlvs = (MTC_TLVS *)malloc(sizeof(MTC_TLVS) + length);
	if (tlvs == NULL)
	{
		return NULL;
	}
	memset(tlvs, 0x00, sizeof(MTC_TLVS) + length);
	tlvs->tag = command;
	tlvs->length = length;
	if (data != NULL)
	{
		memcpy(tlvs->value, data, length);
	}

	return tlvs;
}

/******************************************************************************
Function   : mtc_core_free_tlvs
Description: free tlvs
Input      : the tlvs list
Output     : void
Return     : 0 is success.
******************************************************************************/
int mtc_core_free_tlvs(MTC_TLVS *tlvs)
{
	MTC_TLVS *head = NULL;

	while (tlvs != NULL)
	{
		head = tlvs;
		tlvs = tlvs->next;
		free(head);
	}

	return 0;
}

/******************************************************************************
Function   : mtc_core_parse_tlvs
Description: parse tlvs form data
Input      : the data
Output     : void
Return     : the tlv list
******************************************************************************/
MTC_TLVS *mtc_core_parse_tlvs(unsigned char *data)
{
	MTC_TLVS *head = NULL;
	MTC_TLVS *prev = NULL;
	MTC_TLVS *tlvs = NULL;
	MTC_TLV *tlv = NULL;

	tlv = mtc_package_get_first_tlv(data);
	while (tlv != NULL)
	{
		tlvs = mtc_core_create_tlvs(tlv->tag, tlv->length, tlv->value);
		if (tlvs == NULL)
		{
			mtc_core_free_tlvs(head);
			return NULL;
		}

		if (head == NULL)
		{
			head = tlvs;
		}
		if (prev != NULL)
		{
			prev->next = tlvs;
		}
		prev = tlvs;
		tlv = mtc_package_get_next_tlv(data, tlv);
	}

	return head;
}

/******************************************************************************
Function   : mtc_core_send_request
Description: send tlvs.
Input      : tlvs list
Output     : void
Return     : 0 is sucess, other is fail
******************************************************************************/
int mtc_core_send_tlvs(MTC_TLVS *tlvs, DEV_HANDLE dev)
{
	unsigned char *data = NULL;
	unsigned int length = 0;
	MTC_TLVS *head = tlvs;
	MTC_TLV *tlv = NULL;


	while (tlvs != NULL)
	{
		length += MTC_PACKAGE_TLV_T_LENGTH + MTC_PACKAGE_TLV_L_LENGTH + tlvs->length;
		tlvs = tlvs->next;
	}

	length = MTC_PACKAGE_PROTOCOL_LENGTH + MTC_PACKAGE_LENGTH_LENGTH + length + MTC_PACKAGE_CRC32_LENGTH;
	data = (unsigned char *)malloc(length);
	if (data == NULL)
	{
		return -2;
	}
	memset(data, 0x00, length);

	//    if(service->config.mode == MTC_MODE_UART_SERVER || service->config.mode == MTC_MODE_SOCKET_SERVER)
	//    {
	//        mtc_package_fill_protocol(data, MTC_PACKAGE_PROTOCOL_SERVER);
	//    }
	//    else if(service->config.mode == MTC_MODE_UART_CLIENT || service->config.mode == MTC_MODE_SOCKET_CLIENT)
	//    {
	mtc_package_fill_protocol(data, MTC_PACKAGE_PROTOCOL_CLIENT);
	//    }
	//    else
	//    {
	//        free(data);
	//        return -3;
	//    }
	mtc_package_fill_length(data, 0);
	tlvs = head;
	while (tlvs != NULL)
	{
		tlv = (MTC_TLV *)&(tlvs->tag);
		mtc_package_fill_tlv(data, tlv);
		tlvs = tlvs->next;
	}

	mtc_package_fill_crc32(data);

    if(dev->writeDate((char*)data, length) == -1)
    {
        free(data);
        return -4;
    }
	free(data);

	return 0;
}

MTC_TLVS *mtc_core_send_recv(MTC_TLVS *tlvs, int msecond, DEV_HANDLE dev)
{
	MTC_TLVS *recv_tlvs = nullptr;
	if (mtc_core_send_tlvs(tlvs, dev) == 0)
	{
        if (dev->waitForBytesWritten(msecond))
        {
            if (dev->waitForReadyRead(msecond))
            {
                    QByteArray responseData = dev->readAll();
                    if (mtc_package_validity_check((unsigned char*)responseData.data()) != 0)
                    {
                        std::cerr << "validity check failed!";
                    }
                    else
                    {
                        recv_tlvs = mtc_core_parse_tlvs((unsigned char*)responseData.data());
                        responseData.clear();
                    }
            } else
            {
                qDebug() << "waitForReadyRead mtc_core_send_recv: Wait read response timeout!";
            }
        } else {
            qDebug() << "waitForBytesWritten mtc_core_send_recv: Wait write response timeout!";
        }
	}
	return recv_tlvs;
}

MTC_TLVS *mtc_core_send_recv_big(MTC_TLVS *tlvs, int msecond, DEV_HANDLE dev)
{
    MTC_TLVS *recv_tlvs = nullptr;
    QByteArray responseData;
    if (mtc_core_send_tlvs(tlvs, dev) == 0)
    {
        if (dev->waitForBytesWritten(msecond))
        {
            if (dev->waitForReadyRead(msecond))
            {
                responseData.append(dev->readAll());
                while (dev->waitForReadyRead(30))
                {
                    std::cerr << "mtc_core_send_recv_big responseDataLen"<< responseData.size();
                    responseData.append(dev->readAll());
                }

                if (mtc_package_validity_check((unsigned char*)responseData.data()) != 0)
                {
                    std::cerr << "validity check failed!";
                }
                else
                {
                    recv_tlvs = mtc_core_parse_tlvs((unsigned char*)responseData.data());
                    responseData.clear();
                }
                QThread::msleep(10);

            } else
            {
                qDebug() << "mtc_core_send_recv: Wait read response timeout!";
            }
        } else {
            qDebug() << "mtc_core_send_recv: Wait write response timeout!";
        }
    }
    return recv_tlvs;
}

