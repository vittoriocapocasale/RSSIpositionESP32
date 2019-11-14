/*
 * UsefulInfo.h
 *
 *  Created on: May 8, 2019
 *      Author: vittorio
 */

#ifndef MAIN_USEFULINFO_H_
#define MAIN_USEFULINFO_H_
#include <string>
#include <ctime>
#include <vector>
#include <memory>
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include <string.h>
class UsefulInfo {
public:
	UsefulInfo();
	uint64_t hash;
	uint64_t srcAddr;
	uint64_t dstAddr;
	uint64_t bssid;
	char type[7];
	char ssid[33];
	int8_t intensity;
	uint32_t time;
	std::shared_ptr<std::vector<uint8_t>> bufferize();
private:
	uint64_t htonll(uint64_t n);
};

#endif /* MAIN_USEFULINFO_H_ */
