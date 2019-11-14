/*
 * Callback.cpp
 *
 *  Created on: May 9, 2019
 *      Author: vittorio
 */

#include "Callback.h"

typedef struct {
	uint8_t frameControl[2];
	uint8_t duration[2];
	uint8_t recvAddr[6];
	uint8_t sndrAddr[6];
	uint8_t bssid[6];
	uint8_t seqCtrl[2];
}MACframeHeader;

typedef struct {
    MACframeHeader header;
    uint8_t payload[8000];
} MACframe;

void PacketReceivedCallback (void* arg, esp_event_base_t event_base,int32_t event_id, void* buf)
{
	int64_t srcaddr;
	int64_t dstaddr;
	int64_t bssid;
	char pktype[7];
	int8_t intensity;
	uint32_t time;
	uint64_t hash;
	uint32_t packetLen;
	char ssid[257];
	uint8_t ssidSize;
	uint8_t* ptr;
	int i;

		wifi_promiscuous_pkt_t* wifiPacket = (wifi_promiscuous_pkt_t*)buf;
		MACframe* wifiPayload= (MACframe*)wifiPacket->payload;
	    MACframeHeader * macHead = (MACframeHeader *) &(wifiPayload->header);
        uint8_t mask=0b11110000;
		uint8_t fc0=macHead->frameControl[0];
		uint8_t subtype;
		subtype=fc0 & mask;
		/**/
		intensity=wifiPacket->rx_ctrl.rssi;
		int inttt=0;
		inttt+=intensity;

		/**/
		time=wifiPacket->rx_ctrl.timestamp;
		packetLen=wifiPacket->rx_ctrl.sig_len;
		/**/
		hash=hash64((uint8_t*)wifiPayload, packetLen);
		/**/
		srcaddr=0;
		ptr=(uint8_t*) &srcaddr;
		for(i=0;i<6;i++)
		{
			ptr[2+i]=(macHead->sndrAddr)[i];
		}
		srcaddr=ntohll(srcaddr);
		/**/
		dstaddr=0;
		ptr=(uint8_t*) &dstaddr;
		for(i=0;i<6;i++)
		{
			ptr[2+i]=(macHead->recvAddr)[i];
		}
		dstaddr=ntohll(dstaddr);
		/**/
		bssid=(uint64_t)macHead->bssid;
		bssid=bssid>>16;
		bssid=ntohll(bssid);
		//printf("%02x %02x %02x\n", fc0, macFrame->frameControl[0], macFrame->frameControl[1]);
		if (subtype==0b01000000)//is probe or beacon?
		{
			/**/
			strncpy(pktype,"PRBREQ",6);
			printf("%s", "probe request\n");
			ssidSize=wifiPayload->payload[0];
			for (i=0;i<ssidSize;i++)
			{
				ssid[i]=(char)wifiPayload->payload[i];
			}
			/**/
			ssid[i]='\0';
			//printf("%s", ssid);
			//printf ("%02x:%02x:%02x:%02x:%02x:%02x\n", macHead->sndrAddr[0],macFrame->sndrAddr[1],macFrame->sndrAddr[2],macFrame->sndrAddr[3],macFrame->sndrAddr[4],macFrame->sndrAddr[5]);
		/*else if (subtype==0b01010000)
		{
			strncpy(pktype,"PRBRES",6);
			printf("%s", "probe response\n");
			ssidSize=wifiPayload->payload[13];
			for (i=0;i<ssidSize;i++)
			{
				ssid[i]=wifiPayload->payload[14+i];
			}
			ssid[i]='\0';
			printf("%s\n", ssid);
		}
		else if (subtype==0b10000000)
		{
			strncpy(pktype,"BEACON",6);
			printf("%s", "beacon\n");
			ssidSize=wifiPayload->payload[13];
			for (i=0;i<ssidSize;i++)
			{
				ssid[i]=(char)wifiPayload->payload[14+i];
			}

			ssid[i]='\0';
			printf("%s\n", ssid);
		}*/
			std::shared_ptr<UsefulInfo> ui=std::make_shared<UsefulInfo>();
			ui->srcAddr=srcaddr;
			ui->dstAddr=dstaddr;
			ui->bssid=bssid;
			strncpy(ui->ssid,ssid,32);
			strncpy(ui->type,pktype,6);
			ui->time=time;
			ui->intensity=intensity;
			ui->hash=hash;
			GlobalState::getInstance().buffer.push(ui);
		}

}


uint64_t hash64(const uint8_t* buff, uint32_t buflen)
{
	int i;
	uint64_t ret=17;
	uint64_t* ptr=(uint64_t*)(&(buff[0]));
    for (i=0;i*8+8<=buflen;i++)
    {
    	ret=ret*31+ptr[i];
    }
    return ret;
}

uint64_t ntohll(uint64_t n)
{
    int num = 42;
    if (*(char *)&num == 42)
    {
        return (((uint64_t)ntohl(n)) << 32) + ntohl(n >> 32);
    }
    else
    {
        return n;
    }
}
