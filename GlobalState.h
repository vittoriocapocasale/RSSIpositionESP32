/*
 * GlobalState.h
 *
 *  Created on: May 11, 2019
 *      Author: vittorio
 */

#ifndef MAIN_GLOBALSTATE_H_
#define MAIN_GLOBALSTATE_H_
#include "InfoBuffer.h"
#include <mutex>
#include <atomic>

class GlobalState {
public:
	const char* SADDRESS="192.168.43.124";
	const char* SPORT="4567";
	const uint8_t *ssid =(uint8_t*)"AndroidAP";//{'C', 'a', 'p', 'W', 'i', 'F', 'i','\0'};
    const uint8_t *psw =(uint8_t*)"00000000";//{'a','l','e','s','s','i','a','c','a','p','o','\0'};
    const uint8_t *bssid =(uint8_t*)"";
	static GlobalState& getInstance(){
	    static GlobalState instance;
	    // volatile int dummy{};
	    return instance;
	  }
	GlobalState& operator=(const GlobalState& g)=delete;
	GlobalState(const GlobalState& g)=delete;
	InfoBuffer buffer;
	std::atomic<bool> disconectExpected;
	std::atomic<uint8_t> channel;
	uint8_t macAddr[6];
	esp_timer_handle_t timer;
private:
	GlobalState():channel(8){};
	~GlobalState(){};
};

#endif /* MAIN_GLOBALSTATE_H_ */
