/*
 * InfoBuffer.cpp
 *
 *  Created on: May 9, 2019
 *      Author: vittorio
 */

#include "InfoBuffer.h"
InfoBuffer::InfoBuffer(){
	this->buffer=std::make_shared<std::vector<std::shared_ptr<UsefulInfo>>>();
}


void InfoBuffer::push(std::shared_ptr<UsefulInfo> packet){
	std::lock_guard<std::mutex> l(this->bufferLock);
	if (this->buffer->size()<1000)
	{
		this->buffer->push_back(packet);
	}

}
std::shared_ptr<std::vector<std::shared_ptr<UsefulInfo>>> InfoBuffer::clearAndGet(){
	std::lock_guard<std::mutex> l(this->bufferLock);
	std::shared_ptr<std::vector<std::shared_ptr<UsefulInfo>>> ret=this->buffer;
	this->buffer=std::make_shared<std::vector<std::shared_ptr<UsefulInfo>>>();
	return ret;
}

