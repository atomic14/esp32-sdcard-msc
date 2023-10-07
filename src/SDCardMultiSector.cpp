#include <Arduino.h>
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"

#include "SDCardMultiSector.h"

bool SDCardMultiSector::writeSectors(void *src, size_t start_sector, size_t sector_count) {
  xSemaphoreTake(m_mutex, portMAX_DELAY);
  digitalWrite(GPIO_NUM_2, HIGH);
  esp_err_t res = sdmmc_write_sectors(m_card, src, start_sector, sector_count);
  digitalWrite(GPIO_NUM_2, LOW);
  xSemaphoreGive(m_mutex);
  return res == ESP_OK;
}

bool SDCardMultiSector::readSectors(void *dst, size_t start_sector, size_t sector_count) {
  xSemaphoreTake(m_mutex, portMAX_DELAY);
  digitalWrite(GPIO_NUM_2, HIGH);
  esp_err_t res = sdmmc_read_sectors(m_card, dst, start_sector, sector_count);
  digitalWrite(GPIO_NUM_2, LOW);
  xSemaphoreGive(m_mutex);
  return res == ESP_OK;
}
