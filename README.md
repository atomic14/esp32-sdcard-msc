# Accompanying Video

[![WiFi Streaming](https://img.youtube.com/vi/ocXs1yxsux4/0.jpg)](https://www.youtube.com/watch?v=ocXs1yxsux4)

# ESP32-S3 SD Card Performance Tests - USBMSC and Direct From ESP32

# SD Card Performance Directly Connected to PC

This is just a baseline to see what the SD Card is actually capable of when connected directly to the SD Card reader of my Mac Book Pro.

|   | Average Speed (B/s) | Average Speed (Mbytes/s) | Average Speed (Mbits/s) |
|---|---------------------|--------------------------|-------------------------|
| **Write** | 27,297,357 | 26.03 | 208.26 |
| **Read**  | 94,395,970 | 90.02 | 720.18 |


# ESP32 <-> SD Card Performance

This test write 100MB of raw sectors to the SD Card and then reads it back. Enable this mode ny uncommenting the line in `main.cpp`

```cpp
// #define SD_CARD_SPEED_TEST
```

All values are in MBytes/s

| Operation | 1Bit SPI 20 MHz  | 4Bit SDIO 40 MHz |
| --------- | ---------------- | ---------------- |
| Write     | 1.01             | 2.34             |
| Read      | 1.67             | 8.39             |


# USB <-> ESP32 <-> SD Card: Using USBMSC

You can switch between these different modes by chaning the class used for the SDCard

```cpp
#ifdef USE_SDIO
  card = new SDCardLazyWrite(Serial, "/sd", SD_CARD_CLK, SD_CARD_CMD, SD_CARD_DAT0, SD_CARD_DAT1, SD_CARD_DAT2, SD_CARD_DAT3);
#else
  card = new SDCardLazyWrite(Serial, "/sd", SD_CARD_MISO, SD_CARD_MOSI, SD_CARD_CLK, SD_CARD_CS);
#endif
```

You can use the following classes: `SDCardLazyWrite`, `SDCardMultiWrite`, `SDCardArduino` - NOTE - `SDCardArduino` cannot be used in  SDIO mode as the readRAW and writeRAW functions don't exist on the SDMMC class.
 
## Arduino SD Card Code - Single Sector Writing


| Type        | SPI 20MHz          | 4Bit SDIO 40 MHz |
| ----------- | ------------------ | ---------------- |
| Write Speed | 0.278              | NA              |
| Read Speed  | 0.507              | NA              |


## IDF SD Card Code - Multi Sector Writing


| Type        | SPI 20MHz          | 4Bit SDIO 40 MHz |
| ----------- | ------------------ |------------------ |
| Write Speed | 0.481              |0.66               |
| Read Speed  | 0.671              |1.00               |


## IDF SD Card Code - Multi Sector Lazy Writing

| Type        | SPI 20MHz          | 4Bit SDIO 40 MHz |
| ----------- | ------------------ | ------------------ |
| Write Speed | 0.915              | 0.92               |
| Read Speed  | 0.672              | 1.00               |

