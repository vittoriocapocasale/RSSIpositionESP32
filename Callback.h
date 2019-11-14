/*
 * Callback.h
 *
 *  Created on: May 9, 2019
 *      Author: vittorio
 */

#ifndef MAIN_CALLBACK_H_
#define MAIN_CALLBACK_H_

#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"


#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include "GlobalState.h"
#include "UsefulInfo.h"

#include <memory>

void PacketReceivedCallback (void* arg, esp_event_base_t event_base,int32_t event_id, void* buf);
uint64_t hash64(const uint8_t* buff, uint32_t buflen);
uint64_t ntohll(uint64_t n);
#endif /* MAIN_CALLBACK_H_ */
