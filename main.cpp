#include <iostream>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"

/*
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
*/
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <string.h>
#include <signal.h>
#include <inttypes.h>
#include <netdb.h>

#include "UsefulInfo.h"
#include "GlobalState.h"
#include "Callback.h"

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

#define INBUF_MAX 10

#define INBUF_MAX 1024
#define OUTBUF_MAX 1024
#define COPYBUF_MAX 50
#define LINGER_TIME 30
#define TIMEOUT 30

ssize_t writeNBytes(int s, char *ptr, size_t maxBytes);
ssize_t readNBytes(int s, char *ptr, size_t maxBytes);
int sendData(const char* addressString, const char* portString);
void sendStringMessage(int sock, char* message);
void initWifi();
void startWiFiProm();
void stopWiFiProm();
void startWiFiSta();
void connectWiFiSta();
void disconnectWiFiSta();
void stopWiFiSta();
void startTimer();
int helloMsg (int sock);
int getMsg(int sock);
ESP_EVENT_DECLARE_BASE(CUSTOM_EVENTS);
enum {
    CUSTOM_EVENT_PKRCV,
    CUSTOM_EVENT_TIMER
};
ESP_EVENT_DEFINE_BASE(CUSTOM_EVENTS);



void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{

	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
	{
		//printf("started\n");
		connectWiFiSta();
	}
	if (event_base == IP_EVENT && event_id ==IP_EVENT_STA_GOT_IP)
	{
		//protocol connect;
		//printf("got ip\n");
		sendData(GlobalState::getInstance().SADDRESS, GlobalState::getInstance().SPORT);
		//printf("sent\n");
		startTimer();
		GlobalState::getInstance().disconectExpected.store(true);
		disconnectWiFiSta();
	}
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
	{
		if (GlobalState::getInstance().disconectExpected.exchange(false)==true)
		{

			stopWiFiSta();
		}
		else
		{
			esp_restart();
		}
	}
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_STOP)
	{
		startWiFiProm();
	}
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
	{
			//printf("WHYYYY?\n");
	}
	if (event_base == CUSTOM_EVENTS && event_id == CUSTOM_EVENT_TIMER)
	{
		stopWiFiProm();
		startWiFiSta();
	}
}
/*void esp_event_handler_t (void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{

}*/

void prc(void *buf, wifi_promiscuous_pkt_type_t ttype){
	if(ttype == WIFI_PKT_MGMT)
	{
		PacketReceivedCallback (NULL,CUSTOM_EVENTS, CUSTOM_EVENT_PKRCV,buf );
	}

}
void tec(void *arg){
	//printf("%c", 't');
	esp_event_post(CUSTOM_EVENTS, CUSTOM_EVENT_TIMER, nullptr, 0, 0);
}

extern "C" {
  void app_main(void);
}

void app_main(void)
{

	nvs_flash_init();
	tcpip_adapter_init();

	ESP_ERROR_CHECK( esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_event_handler_register(CUSTOM_EVENTS, CUSTOM_EVENT_TIMER,  event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_START,  event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP,  event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED,  event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_STOP,  event_handler, NULL));

    esp_timer_create_args_t t_args ={};
    t_args.callback=tec;
    t_args.name="timer";
    ESP_ERROR_CHECK(esp_timer_create(&t_args, &(GlobalState::getInstance().timer)));

    esp_efuse_mac_get_default(GlobalState::getInstance().macAddr);
    initWifi();
    unsigned char* c= (unsigned char*) &(GlobalState::getInstance().macAddr);
    printf("\nMAC: ");
    for(int i=0; i< 6;i++)
    {
    	printf("%02X\n", c[i]);
    }
    printf("\n");
    startWiFiSta();
}

void startTimer()
{
	ESP_ERROR_CHECK(esp_timer_start_once(GlobalState::getInstance().timer, 60000000));
}

void initWifi()
{
	wifi_country_t wifi_country = {};
	strncpy(wifi_country.cc,"IT", 2);
	wifi_country.schan=1;
	wifi_country.nchan=13;
	wifi_country.policy=WIFI_COUNTRY_POLICY_AUTO;
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
	ESP_ERROR_CHECK( esp_wifi_set_country(&wifi_country) );
}

void startWiFiProm()
{
	ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_NULL) );
	ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));
	//printf("Chan=%hhu\n", GlobalState::getInstance().channel.load());
	ESP_ERROR_CHECK( esp_wifi_set_channel(GlobalState::getInstance().channel.load(), WIFI_SECOND_CHAN_NONE));
	ESP_ERROR_CHECK(esp_wifi_set_promiscuous_rx_cb(prc));
}
void stopWiFiProm()
{
	ESP_ERROR_CHECK(esp_wifi_set_promiscuous_rx_cb(nullptr));
	ESP_ERROR_CHECK(esp_wifi_set_promiscuous(false));
}

void startWiFiSta()
{

	ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
	wifi_sta_config_t sta={};
	memcpy(sta.ssid, GlobalState::getInstance().ssid, 32*sizeof(uint8_t));
	memcpy(sta.password, GlobalState::getInstance().psw, 64*sizeof(uint8_t));
	memcpy(sta.bssid, GlobalState::getInstance().bssid, 6*sizeof(uint8_t));
	wifi_config_t sta_config = {};
	sta_config.sta=sta;
	ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
	ESP_ERROR_CHECK( esp_wifi_start() );
}
void connectWiFiSta()
{
	ESP_ERROR_CHECK( esp_wifi_connect() );
}

void disconnectWiFiSta()
{
	ESP_ERROR_CHECK( esp_wifi_disconnect() );
}
void stopWiFiSta()
{
	ESP_ERROR_CHECK( esp_wifi_stop() );
}


int sendData(const char* addressString, const char* portString)
{
    int intRet;
    ssize_t ret;
    int sock=-1;
    struct addrinfo hints, *allAddresses, *selectedAddress;
    struct linger lingerStruct;
    struct timeval timeoutR,timeoutS;

    bzero(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    intRet = getaddrinfo(addressString, portString, &hints, &allAddresses);
    if (intRet != 0) {
        printf("Error in getaddrinfo()\n");
        return -2;
    }
    for (selectedAddress=allAddresses; selectedAddress != NULL; selectedAddress=selectedAddress->ai_next)
    {
        sock = socket(selectedAddress->ai_family, selectedAddress->ai_socktype, selectedAddress->ai_protocol);
        if ( sock<0)
        {
            continue;
        }
        lingerStruct.l_onoff=1;
        lingerStruct.l_linger=LINGER_TIME;
        /*ret=setsockopt(sock,SOL_SOCKET,SO_LINGER,(const struct linger*)&lingerStruct,sizeof(lingerStruct));
        if(ret<0)
        {
            printf("\nError in setsockopt(1), continuing ...\n");
            continue;
        }*/
        timeoutS.tv_sec=TIMEOUT;
        timeoutS.tv_usec=1;
        timeoutR.tv_sec=TIMEOUT;
        timeoutR.tv_usec=1;
        ret=setsockopt(sock,SOL_SOCKET,SO_SNDTIMEO,(const struct timeval*)&timeoutS,sizeof(timeoutS));
        if(ret<0)
        {
            printf("\nError in setsockopt(2), continuing...\n");
            continue;
        }
        ret=setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(const struct timeval*)&timeoutR,sizeof(timeoutR));
        if(ret<0)
        {
            printf("\nError in setsockopt(3), continuing ..\n");
            continue;
        }
        ret=connect(sock, selectedAddress->ai_addr, selectedAddress->ai_addrlen);
        if (ret <0)
        {
            close(sock);
            continue;
        }

        // Connected succesfully!
        break;
    }
    freeaddrinfo(allAddresses);
    if (selectedAddress == NULL||sock<1) {
        // The loop wasn't able to connect to the server
        printf("Unable to connect to the server\n.");
        return -6;
    }

    if (helloMsg(sock)<0)
    {
    	close(sock);
    	return -7;
    }
    ret=getMsg(sock);
	//printf("%d\n", ret);
    while (ret==1)
    {
    	ret=getMsg(sock);
    	//printf("%d\n", ret);
    }
    if(ret<0)
    {
    	close(sock);
    	return -8;
    }
    close(sock);
    return 0;
}




int helloMsg (int sock) {
	char  outbuf[OUTBUF_MAX+1];
	char* charPointer=(char*)GlobalState::getInstance().macAddr;
	char* charPointer2;
	ssize_t ret;
	uint32_t vSize, dataSize;
	std::shared_ptr<std::vector<std::shared_ptr<UsefulInfo>>> pkts=GlobalState::getInstance().buffer.clearAndGet();
	std::vector<uint8_t> v;
	for (auto it=pkts->begin(); it!=pkts->end(); it++)
	{
		uint8_t tmp;
		//printf("G\n");
		std::shared_ptr<std::vector<uint8_t>> pkt=(*it)->bufferize();
		//printf("H\n");
		tmp=pkt->size();		
		v.push_back(tmp);
		v.insert(v.end(), pkt->begin(), pkt->end());
	}
	//printf("I\n");
	dataSize=htonl(v.size()+10);
	//printf("P\n");
	charPointer2=(char*) &dataSize;
	//printf("O\n");
	snprintf(outbuf, OUTBUF_MAX , "%c%c%c%c",charPointer2[0], charPointer2[1], charPointer2[2], charPointer2[3]);
	//printf("N\n");
	ret=writeNBytes(sock,outbuf,4);
	//printf("M\n");
	if(ret!=4)
	{
		printf("%s\n","\nError in write0(), continuing looping...\n");
		return -1;
	}
	//printf("L\n");
	vSize=htonl(pkts->size());
	//printf("Q\n");
	charPointer2=(char*) &vSize;
	//printf("vLength:%d\n", v.size());
	//printf("vSize:%d\n", pkts->size());
	//printf("vSizet:%d\n", vSize);
	//printf("K\n");
	snprintf(outbuf, OUTBUF_MAX , "%c%c%c%c%c%c%c%c%c%c",
			charPointer[0], charPointer[1], charPointer[2], charPointer[3], charPointer[4], charPointer[5],
			charPointer2[0], charPointer2[1], charPointer2[2], charPointer2[3]);
	ret=writeNBytes(sock,outbuf,10);
	for(int i=0; i<10;i++)
	{
		printf("%02X", outbuf[i]);
	}
	if(ret!=10)
	{
		printf("%s\n","\nError in write(), continuing looping...\n");
		return -1;
	}
	int currentTransferred=0, totalTransferred=0, allOK=1, bufferSize;
	for(totalTransferred=0;totalTransferred<v.size()&&allOK==1;totalTransferred+=currentTransferred)
	{
		bufferSize=v.size()-totalTransferred;
		if(bufferSize>OUTBUF_MAX)
		{
			bufferSize=OUTBUF_MAX;
		}
		memcpy (outbuf, &(v[totalTransferred]),bufferSize);
		currentTransferred=writeNBytes(sock,outbuf,bufferSize);
		if(currentTransferred!=bufferSize)
		{
			printf("%s\n","\nError in write() to socket, continuing looping...\n");
			allOK=0;
			return -1;
		}

	}
	//printf("J\n");
	return 1;
}

int getMsg(int sock) {
	char inbuf[INBUF_MAX+1];
	ssize_t ret;
	uint8_t chan;
	//printf("E\n");
	ret=readNBytes(sock,inbuf,2);
	if(ret!=2)
	{
		printf("\nCannot read size from socket, skipping...\n");
		return -1;
	}
	//printf("F\n");
	chan=(uint8_t)inbuf[1];
	inbuf[1]='\0';
	if (strncmp(inbuf, "W",1)==0)
	{
		return 1;
	}
	else if (strncmp(inbuf,"C",1)==0)
	{
		GlobalState::getInstance().channel.store(chan);
		//printf("Stored:%hhu\n",GlobalState::getInstance().channel.load());
		return 2;
	}
	return -1;
}

ssize_t readNBytes (int s, char *ptr, size_t maxBytes)
{
	ssize_t currentRed;
	size_t bytesLeft;
	ssize_t totalRed=0;
	for (bytesLeft=maxBytes; bytesLeft > 0; )
	{
		currentRed=recv(s, ptr+totalRed, bytesLeft, 0);
        	if (currentRed > 0)
        	{
            		bytesLeft -= currentRed;
            		totalRed += currentRed;
        	}
        	else if (currentRed == 0)
        	{
            		return totalRed;
        	}
        	else
        	{
            		return currentRed;
        	}
    	}
	return totalRed;
}

ssize_t writeNBytes(int s, char *ptr, size_t maxBytes)
{
    size_t bytesLeft;
    ssize_t totalWritten=0;
    ssize_t currentWritten;
    for (bytesLeft=maxBytes; bytesLeft > 0; )
    {
        currentWritten = send(s, ptr+totalWritten, bytesLeft,0);
        if (currentWritten >0)
        {
            bytesLeft -= currentWritten;
            totalWritten += currentWritten;
        }
        else
        {
            return currentWritten;
        }
    }
    return totalWritten;
}

