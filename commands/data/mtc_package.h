#pragma once
#define MTC_PACKAGE_PROTOCOL_SERVER		0xA1
#define MTC_PACKAGE_PROTOCOL_CLIENT		0x1A

#define MTC_PACKAGE_PROTOCOL_LENGTH		1
#define MTC_PACKAGE_LENGTH_LENGTH		4
#define MTC_PACKAGE_CRC32_LENGTH		4
#define MTC_PACKAGE_TLV_T_LENGTH		2
#define MTC_PACKAGE_TLV_L_LENGTH		4

#pragma pack (push,1)
typedef struct MTC_TLV
{
	unsigned short tag;
	unsigned int length;
	unsigned char value[1];
}MTC_TLV;
#pragma pack(pop)
//}__attribute__((packed)) MTC_TLV;

int mtc_package_validity_check(unsigned char *data);

unsigned char mtc_package_get_protocol(unsigned char *data);
unsigned int mtc_package_get_length(unsigned char *data);
MTC_TLV *mtc_package_get_first_tlv(unsigned char *data);
MTC_TLV *mtc_package_get_next_tlv(unsigned char *data, MTC_TLV *tlv);
unsigned int mtc_package_get_crc32(unsigned char *data);

int mtc_package_fill_protocol(unsigned char *data, unsigned char protocol);
int mtc_package_fill_length(unsigned char *data, unsigned int length);
int mtc_package_fill_tlv(unsigned char *data, MTC_TLV *tlvs);
int mtc_package_fill_crc32(unsigned char *data);
