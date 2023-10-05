#pragma once

#include <freertos/FreeRTOS.h>
#include <driver/sdmmc_types.h>
#include <driver/sdspi_host.h>

#include <string>

class SDCard
{
private:
  std::string m_mount_point;
  sdmmc_card_t *m_card;
  sdmmc_host_t m_host = SDSPI_HOST_DEFAULT();
  int m_sector_size = 0;
  int m_sector_count = 0;
  Stream &m_debug;
  // control access to the SD card
  SemaphoreHandle_t m_mutex;
  // queue up requests
  QueueHandle_t m_request_queue;
  // results of reading data
  QueueHandle_t m_read_queue;
  void drainQueue();
public:
  SDCard(Stream &debug, const char *mount_point, gpio_num_t miso, gpio_num_t mosi, gpio_num_t clk, gpio_num_t cs);
  ~SDCard();
  bool writeSectors(void *src, size_t start_sector, size_t sector_count);
  bool readSectors(void *dst, size_t start_sector, size_t sector_count);
  size_t getSectorSize() { return m_sector_size; }
  size_t getSectorCount() { return m_sector_count; }
  void printCardInfo();
  const std::string &get_mount_point() { return m_mount_point; }
};