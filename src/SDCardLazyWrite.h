#pragma once

#include "SDCard.h"

class SDCardLazyWrite: public SDCard
{
private:
  // queue up requests
  QueueHandle_t m_request_queue;
  // results of reading data
  QueueHandle_t m_read_queue;
  void drainQueue();
public:
  SDCardLazyWrite(Stream &debug, const char *mount_point, gpio_num_t miso, gpio_num_t mosi, gpio_num_t clk, gpio_num_t cs);
  ~SDCardLazyWrite();
  bool writeSectors(void *src, size_t start_sector, size_t sector_count);
  bool readSectors(void *dst, size_t start_sector, size_t sector_count);
};