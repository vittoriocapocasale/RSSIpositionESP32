/*
/home/vittorio/esp/esp-idf/components/lwip/lwip/src/include/lwip/def.h:110:29: note: in definition of macro 'htonl'
 #define htonl(x) lwip_htonl(x)
                             ^
/home/vittorio/esp/malnati/cpp3/main/main.c
 * InfoBuffer.h
 *
 *  Created on: May 9, 2019
 *      Author: vittorio
 */

#ifndef MAIN_INFOBUFFER_H_
#define MAIN_INFOBUFFER_H_

#include <memory>
#include <mutex>
#include <vector>
#include "UsefulInfo.h"
class InfoBuffer {
public:
  InfoBuffer();
  ~InfoBuffer()=default;
  InfoBuffer(const InfoBuffer &)=delete;
  InfoBuffer& operator= (const InfoBuffer &)=delete;
  void push(std::shared_ptr<UsefulInfo> packet);
  std::shared_ptr<std::vector<std::shared_ptr<UsefulInfo>>>  clearAndGet();


private:
	std::mutex bufferLock;
	std::shared_ptr<std::vector<std::shared_ptr<UsefulInfo>>> buffer;
};

#endif /* MAIN_INFOBUFFER_H_ */
