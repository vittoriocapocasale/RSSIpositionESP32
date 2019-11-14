/*
 * UsefulInfo.cpp
 *
 *  Created on: May 8, 2019
 *      Author: vittorio
 */

#include "UsefulInfo.h"

UsefulInfo::UsefulInfo() {
	// TODO Auto-generated constructor stub

}
std::shared_ptr<std::vector<uint8_t>> UsefulInfo::bufferize()
{
	int i;
	uint8_t *ptr;
	uint64_t temp64;
	uint8_t temp8;
	uint32_t temp32;
	std::shared_ptr<std::vector<uint8_t>> buf= std::make_shared<std::vector<uint8_t>> ();

	temp64=UsefulInfo::htonll(this->hash);
	ptr=(uint8_t*) &temp64;
	buf->push_back(ptr[0]);
	buf->push_back(ptr[1]);
	buf->push_back(ptr[2]);
	buf->push_back(ptr[3]);
	buf->push_back(ptr[4]);
	buf->push_back(ptr[5]);
	buf->push_back(ptr[6]);
	buf->push_back(ptr[7]);

	temp64=UsefulInfo::htonll(this->srcAddr);
	ptr=(uint8_t*) &temp64;
	buf->push_back(ptr[0]);
	buf->push_back(ptr[1]);
	buf->push_back(ptr[2]);
	buf->push_back(ptr[3]);
	buf->push_back(ptr[4]);
	buf->push_back(ptr[5]);
	buf->push_back(ptr[6]);
	buf->push_back(ptr[7]);

	/*temp64=UsefulInfo::htonll(this->dstAddr);
	ptr=(uint8_t*) &temp64;
	buf->push_back(ptr[0]);
	buf->push_back(ptr[1]);
	buf->push_back(ptr[2]);
	buf->push_back(ptr[3]);
	buf->push_back(ptr[4]);
	buf->push_back(ptr[5]);
	buf->push_back(ptr[6]);
	buf->push_back(ptr[7]);*/

	/*temp64=UsefulInfo::htonll(this->bssid);
	ptr=(uint8_t*) &temp64;
	buf->push_back(ptr[0]);
	buf->push_back(ptr[1]);
	buf->push_back(ptr[2]);
	buf->push_back(ptr[3]);
	buf->push_back(ptr[4]);
	buf->push_back(ptr[5]);
	buf->push_back(ptr[6]);
	buf->push_back(ptr[7]);*/



	/*ptr=(uint8_t*) this->type;
	buf->push_back(ptr[0]);
	buf->push_back(ptr[1]);
	buf->push_back(ptr[2]);
	buf->push_back(ptr[3]);
	buf->push_back(ptr[4]);
	buf->push_back(ptr[5]);
	buf->push_back('\0');*/

	temp8=this->intensity;
	ptr=(uint8_t*) &temp8;
	buf->push_back(ptr[0]);

	temp32=htonl(this->time);
	ptr=(uint8_t*) &temp32;
	buf->push_back(ptr[0]);
	buf->push_back(ptr[1]);
	buf->push_back(ptr[2]);
	buf->push_back(ptr[3]);

	temp8=strlen(this->ssid);
	ptr=(uint8_t*) &temp8;
	buf->push_back(ptr[0]);


	ptr=(uint8_t*) this->ssid;
	for (i=0; i<strlen(this->ssid); i++)
	{
		buf->push_back(ptr[i]);
	}
	//buf->push_back('\0');
	return buf;
}

uint64_t UsefulInfo::htonll(uint64_t n)
{
    static const int num = 42;
    // Check the endianness
    if (*(char *)&num == 42)
    {
        return (((uint64_t)htonl(n)) << 32) + htonl(n >> 32);
    }
    else
    {
        return n;
    }
}
