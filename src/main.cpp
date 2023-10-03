#include <Arduino.h>
#include "USB.h"
#include "USBHIDKeyboard.h"
#include "MySD.h"
#include "SPI.h"
#include "USBMSC.h"
#include "sd_diskio.h"

#define BOOT_BUTTON 0

USBHIDKeyboard keyboard;
USBMSC msc;
USBCDC Serial;

void log(const char *str)
{
    Serial.println(str);
}

static int32_t onWrite(uint32_t lba, uint32_t offset, uint8_t *buffer, uint32_t bufsize)
{
    digitalWrite(GPIO_NUM_2, HIGH);
    // Serial.printf("Writing %d bytes to %d at offset\n", bufsize, lba, offset);
    // this writes a complete sector so we should return sector size on success
    if (MySD.writeRAW((uint8_t *)buffer, lba, bufsize/512))
    {
        digitalWrite(GPIO_NUM_2, LOW);
        return bufsize;
    }
    digitalWrite(GPIO_NUM_2, LOW);
    return bufsize;
    // return -1;
}

static int32_t onRead(uint32_t lba, uint32_t offset, void *buffer, uint32_t bufsize)
{
    digitalWrite(GPIO_NUM_2, HIGH);
    // Serial.printf("Reading %d bytes from %d at offset %d\n", bufsize, lba, offset);
    // this reads a complete sector so we should return sector size on success
    if (MySD.readRAW((uint8_t *)buffer, lba, bufsize/512))
    {
        digitalWrite(GPIO_NUM_2, LOW);
        return bufsize;
    }
    digitalWrite(GPIO_NUM_2, LOW);
    return -1;
}

static bool onStartStop(uint8_t power_condition, bool start, bool load_eject)
{
    Serial.printf("StartStop: %d %d %d\n", power_condition, start, load_eject);
    return true;
}

bool isBootButtonClicked()
{
    return digitalRead(BOOT_BUTTON) == LOW;
}

void setup()
{
    pinMode(GPIO_NUM_2, OUTPUT);
    pinMode(BOOT_BUTTON, INPUT_PULLUP);

    static SPIClass spi(HSPI);
    spi.begin(SD_CARD_CLK, SD_CARD_MISO, SD_CARD_MOSI, SD_CARD_CS);
    if (MySD.begin(SD_CARD_CS, spi, 80000000))
    {
        log("SD begin");
    }

    // keyboard.begin();
    msc.vendorID("ESP32");
    msc.productID("USB_MSC");
    msc.productRevision("1.0");
    msc.onRead(onRead);
    msc.onWrite(onWrite);
    msc.onStartStop(onStartStop);
    msc.mediaPresent(true);

    msc.begin(MySD.numSectors(), MySD.sectorSize()); //?
    Serial.begin(115200);
    keyboard.begin();
    USB.begin();

    char buff[50];
    sprintf(buff, "Total bytes: %d, Card Size: %d", MySD.totalBytes() / 1024, MySD.cardSize());
    log(buff);
}

void loop()
{
    // put your main code here, to run repeatedly:
    delay(200);

    if (isBootButtonClicked())
    {
        if (MySD.cardType() == CARD_NONE)
        {
            log("No SD card");
        }
    }
}