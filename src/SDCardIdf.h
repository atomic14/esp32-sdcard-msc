#pragma once

#include "SDCard.h"

#include <freertos/FreeRTOS.h>
#include <driver/sdmmc_types.h>
#include <driver/sdspi_host.h>

class SDCardIdf: public SDCard
{
protected:
  sdmmc_card_t *m_card;
  sdmmc_host_t m_host = SDSPI_HOST_DEFAULT();
  // control access to the SD card
  SemaphoreHandle_t m_mutex;
public:
  SDCardIdf(Stream &debug, const char *mount_point, gpio_num_t miso, gpio_num_t mosi, gpio_num_t clk, gpio_num_t cs);
  ~SDCardIdf();
  void printCardInfo();
};
