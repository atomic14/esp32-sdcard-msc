#pragma once

#include "SDCard.h"

class SDCardMultiSector: public SDCard
{
protected:
public:
  bool writeSectors(void *src, size_t start_sector, size_t sector_count);
  bool readSectors(void *dst, size_t start_sector, size_t sector_count);
};