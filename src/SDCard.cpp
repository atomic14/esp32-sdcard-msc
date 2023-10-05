#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"

#include "SDCard.h"

static const char *TAG = "SDC";

#define SPI_DMA_CHAN SPI_DMA_CH_AUTO

enum class RequestType {
  READ,
  WRITE
};

class Request {
public:
  Request(RequestType type, void *data, size_t start_sector, size_t sector_count) : m_type(type), m_start_sector(start_sector), m_sector_count(sector_count) {
    if (type == RequestType::WRITE) {
      m_data = malloc(sector_count * 512);
      memcpy(m_data, data, sector_count * 512);
    } else if(type == RequestType::READ) {
      m_data = data;
    } else {
      m_data = NULL;
    }
  }
  ~Request() {
    if (m_type == RequestType::WRITE) {
      free(m_data);
    }
  }
  RequestType m_type;
  void *m_data;
  size_t m_start_sector;
  size_t m_sector_count;
};

SDCard::SDCard(Stream &debug, const char *mount_point, gpio_num_t miso, gpio_num_t mosi, gpio_num_t clk, gpio_num_t cs) : m_debug(debug)
{
  // a mutex to prevent read and write overlapping
  m_mutex = xSemaphoreCreateMutex();
  // a queue to hold requests (to read or write)
  m_request_queue = xQueueCreate(10, sizeof(Request *));
  // a queue to hold the results of read requests
  m_read_queue = xQueueCreate(10, sizeof(Request *));
  // create a task to drain the write queue
  xTaskCreate([](void *param) {
    SDCard *card = (SDCard *)param;
    card->drainQueue();
  }
  , "SDCard", 4096, this, 1, NULL);

  m_host.max_freq_khz = 80000;
  m_mount_point = mount_point;
  esp_err_t ret;
  // Options for mounting the filesystem.
  // If format_if_mount_failed is set to true, SD card will be partitioned and
  // formatted in case when mounting fails.
  esp_vfs_fat_sdmmc_mount_config_t mount_config = {
      .format_if_mount_failed = false,
      .max_files = 5,
      .allocation_unit_size = 16 * 1024};

  m_debug.println("Initializing SD card");

  spi_bus_config_t bus_cfg = {
      .mosi_io_num = mosi,
      .miso_io_num = miso,
      .sclk_io_num = clk,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
      .max_transfer_sz = 16384,
      .flags = 0,
      .intr_flags = 0};
  ret = spi_bus_initialize(spi_host_device_t(m_host.slot), &bus_cfg, SPI_DMA_CHAN);
  if (ret != ESP_OK)
  {
    m_debug.println("Failed to initialize bus.");
    return;
  }

  // This initializes the slot without card detect (CD) and write protect (WP) signals.
  // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
  sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
  slot_config.gpio_cs = cs;
  slot_config.host_id = spi_host_device_t(m_host.slot);

  ret = esp_vfs_fat_sdspi_mount(m_mount_point.c_str(), &m_host, &slot_config, &mount_config, &m_card);

  if (ret != ESP_OK)
  {
    if (ret == ESP_FAIL)
    {
      m_debug.println("Failed to mount filesystem. "
                    "If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
    }
    else
    {
      m_debug.printf("Failed to initialize the card (%s). "
                    "Make sure SD card lines have pull-up resistors in place.\n",
               esp_err_to_name(ret));
    }
    return;
  }
  m_debug.printf("SDCard mounted at: %s\n", m_mount_point.c_str());
  m_sector_count = m_card->csd.capacity;
  m_sector_size = m_card->csd.sector_size;
  m_debug.printf("SDCard sector count: %d, size: %d\n", m_sector_count, m_sector_size);
  // Card has been initialized, print its properties
  sdmmc_card_print_info(stdout, m_card);
}

SDCard::~SDCard()
{
  // lock the SD card
  xSemaphoreTake(m_mutex, portMAX_DELAY);
  // All done, unmount partition and disable SDMMC or SPI peripheral
  esp_vfs_fat_sdcard_unmount(m_mount_point.c_str(), m_card);
  m_debug.println("Card unmounted");
  //deinitialize the bus after all devices are removed
  spi_bus_free(spi_host_device_t(m_host.slot));
  // unlock the SD card
  xSemaphoreGive(m_mutex);
}

void SDCard::printCardInfo()
{
  sdmmc_card_print_info(stdout, m_card);
}

bool SDCard::writeSectors(void *src, size_t start_sector, size_t sector_count) {
  xSemaphoreTake(m_mutex, portMAX_DELAY);
  // push the write request onto the queue
  void *data = malloc(sector_count * m_sector_size);
  memcpy(data, src, sector_count * m_sector_size);
  Request *req = new Request(RequestType::WRITE, data, start_sector, sector_count);
  xQueueSend(m_request_queue, &req, portMAX_DELAY);
  xSemaphoreGive(m_mutex);
  return true;
}

void SDCard::drainQueue() {
  Request *req;
  while (xQueueReceive(m_request_queue, &req, portMAX_DELAY) == pdTRUE) {
    // lock the SD card
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    digitalWrite(GPIO_NUM_2, HIGH);
    if (req->m_type == RequestType::WRITE) {
      esp_err_t res = sdmmc_write_sectors(m_card, req->m_data, req->m_start_sector, req->m_sector_count);
      delete req;
    } else if (req->m_type == RequestType::READ) {
      esp_err_t res = sdmmc_read_sectors(m_card, req->m_data, req->m_start_sector, req->m_sector_count);
      xQueueSend(m_read_queue, &req, portMAX_DELAY);
    }
    digitalWrite(GPIO_NUM_2, LOW);
    xSemaphoreGive(m_mutex);
  }
}

bool SDCard::readSectors(void *dst, size_t start_sector, size_t sector_count) {
  xSemaphoreTake(m_mutex, portMAX_DELAY);
  // check to see if the queue has any pending writes
  if (uxQueueMessagesWaiting(m_request_queue) > 0) {
    // push our read request onto the queue and wait for it to complete
    Request *req = new Request(RequestType::READ, dst, start_sector, sector_count);
    xQueueSend(m_request_queue, &req, portMAX_DELAY);
    // wait for the read to complete
    xQueueReceive(m_read_queue, &req, portMAX_DELAY);
    delete req;
  } else {
    digitalWrite(GPIO_NUM_2, HIGH);
    // no pending writes, so we can just read directly
    esp_err_t res = sdmmc_read_sectors(m_card, dst, start_sector, sector_count);
    digitalWrite(GPIO_NUM_2, LOW);
  }
  xSemaphoreGive(m_mutex);
  return true;
}
