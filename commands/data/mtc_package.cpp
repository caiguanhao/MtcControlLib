/************************************************************************
Copyright  : 2018-2028,Shenzhen SenseTime Co.,Ltd
File name  : mtc_package.cpp
Description: the tlv package of mtc.
Author     : pengenhou
Date       : 2019/06/12
Version    : V1.0.0
History    : 2019/06/12, Create it by pengenhou
************************************************************************/

//#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>
#include <string.h>

#include "mtc_package.h"

/******************************************************************************
Function   : mtc_package_calc_crc32
Description: calc the crc32.
Input      : package data
			 the length of data
Output     : void
Return     : the crc32 code
******************************************************************************/
unsigned int mtc_package_calc_crc32(unsigned char *data, unsigned int length)
{
	unsigned int crc32_table[256] = { 0 };
	unsigned int crc32 = 0;
	unsigned int index = 0;
	unsigned int pos = 0;

	for (index = 0; index < 256; index++)
	{
		crc32 = index;
		for (pos = 0; pos < 8; pos++)
		{
			if (crc32 & 1)
				crc32 = 0xedb88320L ^ (crc32 >> 1);
			else
				crc32 = crc32 >> 1;
		}
		crc32_table[index] = crc32;
	}

	crc32 = 0xffffffff;
	for (index = 0; index < length; index++)
	{
		crc32 = crc32_table[(crc32 ^ data[index]) & 0xff] ^ (crc32 >> 8);
	}

	return crc32;
}

/******************************************************************************
Function   : mtc_package_validity_check
Description: check the data validity.
Input      : package data
			 the length of data
Output     : void
Return     : 0 is sucess, other is fail
******************************************************************************/
int mtc_package_validity_check(unsigned char *data)
{
	unsigned char protocol = mtc_package_get_protocol(data);

	if (protocol != 0xA1 && protocol != 0x1A)
	{
		return -1;
	}

	unsigned int length = mtc_package_get_length(data);
	unsigned int crc32 = mtc_package_get_crc32(data);

	if (mtc_package_calc_crc32(data, length + MTC_PACKAGE_PROTOCOL_LENGTH + MTC_PACKAGE_LENGTH_LENGTH) != crc32)
	{
		return -2;
	}

	return 0;
}

/******************************************************************************
Function   : mtc_package_get_protocol
Description: get protocol tag from data.
Input      : package data
Output     : void
Return     : the protocol tag
******************************************************************************/
unsigned char mtc_package_get_protocol(unsigned char *data)
{
	return data[0];
}

/******************************************************************************
Function   : mtc_package_get_protocol
Description: get tlv length from data.
Input      : package data
Output     : void
Return     : the protocol tag
******************************************************************************/
unsigned int mtc_package_get_length(unsigned char *data)
{
	return (data[1] << 24 | data[2] << 16 | data[3] << 8 | data[4]);
}

/******************************************************************************
Function   : mtc_package_get_first_tlv
Description: get first tlv from data.
Input      : package data
Output     : void
Return     : the tlv
******************************************************************************/
MTC_TLV *mtc_package_get_first_tlv(unsigned char *data)
{
    MTC_TLV *tlv = nullptr;

	if (mtc_package_get_length(data) > 0)
	{
		tlv = (MTC_TLV *)(data + MTC_PACKAGE_PROTOCOL_LENGTH + MTC_PACKAGE_LENGTH_LENGTH);
	}

	return tlv;
}

/******************************************************************************
Function   : mtc_package_get_next_tlv
Description: get next tlv from data.
Input      : package data
Output     : void
Return     : the tlv
******************************************************************************/
MTC_TLV *mtc_package_get_next_tlv(unsigned char *data, MTC_TLV *tlv)
{
    MTC_TLV *tlv_next = nullptr;
	int tlv_length = 0;
	unsigned char *data_pos = (unsigned char *)tlv;

	tlv_length = mtc_package_get_length(data);

	if (data_pos + MTC_PACKAGE_TLV_T_LENGTH + MTC_PACKAGE_TLV_L_LENGTH + tlv->length - data < MTC_PACKAGE_PROTOCOL_LENGTH + MTC_PACKAGE_LENGTH_LENGTH + tlv_length)
	{
		tlv_next = (MTC_TLV *)(data_pos + MTC_PACKAGE_TLV_T_LENGTH + MTC_PACKAGE_TLV_L_LENGTH + tlv->length);
	}

	return tlv_next;
}

/******************************************************************************
Function   : mtc_package_get_crc32
Description: get next tlv from data.
Input      : package data
Output     : void
Return     : the crc32
******************************************************************************/
unsigned int mtc_package_get_crc32(unsigned char *data)
{
	unsigned char *crc32 = data + MTC_PACKAGE_PROTOCOL_LENGTH + MTC_PACKAGE_LENGTH_LENGTH + mtc_package_get_length(data);

	return (crc32[0] << 24 | crc32[1] << 16 | crc32[2] << 8 | crc32[3]);
}

/******************************************************************************
Function   : mtc_package_fill_protocol
Description: fill the protocol into data.
Input      : the data buffer
			 the protocol head
Output     : void
Return     : 0 is sucess, other is fail
******************************************************************************/
int mtc_package_fill_protocol(unsigned char *data, unsigned char protocol)
{
	if (protocol == 0xA1 || protocol == 0x1A)
	{
		data[0] = protocol;
	}

	return 0;
}

/******************************************************************************
Function   : mtc_package_fill_length
Description: fill the length into data.
Input      : the data buffer
			 the tlvs length
Output     : void
Return     : 0 is sucess, other is fail
******************************************************************************/
int mtc_package_fill_length(unsigned char *data, unsigned int length)
{
	data[1] = (length & 0xFF000000) >> 24;
	data[2] = (length & 0x00FF0000) >> 16;
	data[3] = (length & 0x0000FF00) >> 8;
	data[4] = (length & 0x000000FF);

	return 0;
}

/******************************************************************************
Function   : mtc_package_fill_tlv
Description: fill the tlv into data.
Input      : the data buffer
			 the tlvs list
Output     : void
Return     : 0 is sucess, other is fail
******************************************************************************/
int mtc_package_fill_tlv(unsigned char *data, MTC_TLV *tlv)
{
	unsigned int length = mtc_package_get_length(data);
    MTC_TLV *tlv_next = nullptr;

	tlv_next = (MTC_TLV *)(data + MTC_PACKAGE_PROTOCOL_LENGTH + MTC_PACKAGE_LENGTH_LENGTH + length);
	tlv_next->tag = tlv->tag;
	tlv_next->length = tlv->length;
	memcpy(tlv_next->value, tlv->value, tlv->length);

	length += MTC_PACKAGE_TLV_T_LENGTH + MTC_PACKAGE_TLV_L_LENGTH + tlv->length;
	mtc_package_fill_length(data, length);

	return 0;
}

/******************************************************************************
Function   : mtc_package_fill_crc32
Description: fill the crc32 into data.
Input      : the data buffer
Output     : void
Return     : 0 is sucess, other is fail
******************************************************************************/
int mtc_package_fill_crc32(unsigned char *data)
{
	unsigned int length = mtc_package_get_length(data);
	unsigned int crc32 = mtc_package_calc_crc32(data, length + MTC_PACKAGE_PROTOCOL_LENGTH + MTC_PACKAGE_LENGTH_LENGTH);
	unsigned char *crc32_data = data + MTC_PACKAGE_PROTOCOL_LENGTH + MTC_PACKAGE_LENGTH_LENGTH + length;

	crc32_data[0] = (crc32 & 0xFF000000) >> 24;
	crc32_data[1] = (crc32 & 0x00FF0000) >> 16;
	crc32_data[2] = (crc32 & 0x0000FF00) >> 8;
	crc32_data[3] = (crc32 & 0x000000FF);

	return 0;
}
