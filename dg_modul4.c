/*
 * Copyright (c) 2015-2025, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>
#include <stdbool.h>
#include "common.h"
#include "dgtable.h"
#include "dg_modul4.h"
#include "ramckmdl.h"
#include "io.h"
#include "dg_modul1.h"
#include "bit.h"
#include "spiflash1drv.h"

FlashDevice gFlashDevice = {0, 0, 0, 0};

const FlashDeviceConfig cypressDevices[] __attribute__((section(".rodata"))) = {
    {DEVICE_ID_S25FS512S, "S25FS512S", SA_256KB, TOTAL_SIZE_64MB},
    {DEVICE_ID_S25FS128S, "S25FS128S", SA_256KB, TOTAL_SIZE_16MB}
};

const FlashDeviceConfig winbondDevices[] __attribute__((section(".rodata"))) = {
    {DEVICE_ID_W25Q64JV, "W25Q64JV", SA_64KB, TOTAL_SIZE_8MB},
    {DEVICE_ID_W25Q64JW, "W25Q64JW", SA_64KB, TOTAL_SIZE_8MB},
    {DEVICE_ID_W25Q128JV, "W25Q128JV", SA_64KB, TOTAL_SIZE_16MB},
    {DEVICE_ID_W25Q128JW, "W25Q128JW", SA_64KB, TOTAL_SIZE_16MB},
    {DEVICE_ID_W25Q256, "W25Q256", SA_64KB, TOTAL_SIZE_32MB},
    {DEVICE_ID_W25M512JV, "W25M512JV", SA_64KB, TOTAL_SIZE_32MB},
    {DEVICE_ID_W25M512JW, "W25M512JW", SA_64KB, TOTAL_SIZE_32MB},
    {DEVICE_ID_W25Q512JV, "W25Q512JV", SA_64KB, TOTAL_SIZE_64MB},
    {DEVICE_ID_W25Q512JV_DTR, "W25Q512JV-DTR", SA_64KB, TOTAL_SIZE_64MB}
};

const FlashDeviceConfig macronixDevices[] __attribute__((section(".rodata"))) = {
    {DEVICE_ID_MX25L12805, "MX25L12805", SA_64KB, TOTAL_SIZE_16MB},
    {DEVICE_ID_MX25L25645G, "MX25L25645G", SA_64KB, TOTAL_SIZE_32MB},
    {DEVICE_ID_MX25L51245G, "MX25L51245G", SA_64KB, TOTAL_SIZE_64MB},
    {DEVICE_ID_MX25U12835E, "MX25U12835E", SA_64KB, TOTAL_SIZE_16MB},
    {DEVICE_ID_MX66U25635F, "MX66U25635F", SA_64KB, TOTAL_SIZE_32MB},
    {DEVICE_ID_MX66U51235F, "MX66U51235F", SA_64KB, TOTAL_SIZE_64MB},
    {DEVICE_ID_MX66UM1G45G, "MX66UM1G45G", SA_64KB, TOTAL_SIZE_128MB},
    {DEVICE_ID_MX66UW1G45G, "MX66UW1G45G", SA_64KB, TOTAL_SIZE_128MB}
};

const FlashDeviceConfig micronDevices[] __attribute__((section(".rodata"))) = {
    {DEVICE_ID_MT25QL128, "MT25QL128", SA_64KB, TOTAL_SIZE_16MB},
    {DEVICE_ID_MT25QU128, "MT25QU128", SA_64KB, TOTAL_SIZE_16MB},
    {DEVICE_ID_MT25QL256, "MT25QL256", SA_64KB, TOTAL_SIZE_32MB},
    {DEVICE_ID_MT25QU256, "MT25QU256", SA_64KB, TOTAL_SIZE_32MB},
    {DEVICE_ID_MT25QL512, "MT25QL512", SA_64KB, TOTAL_SIZE_64MB},
    {DEVICE_ID_MT25QU512, "MT25QU512", SA_64KB, TOTAL_SIZE_64MB},
    {DEVICE_ID_MT25QL01G, "MT25QL01G", SA_64KB, TOTAL_SIZE_128MB},
    {DEVICE_ID_MT25QU01G, "MT25QU01G", SA_64KB, TOTAL_SIZE_128MB},
    {DEVICE_ID_MT25QL02G, "MT25QL02G", SA_64KB, TOTAL_SIZE_256MB},
    {DEVICE_ID_MT25QU02G, "MT25QU02G", SA_64KB, TOTAL_SIZE_256MB}
};

const FlashDeviceConfig renesasDevices[] __attribute__((section(".rodata"))) = {
    {DEVICE_ID_AT25QL128A, "AT25QL128A", SA_64KB, TOTAL_SIZE_16MB},
    {DEVICE_ID_AT25SF128A, "AT25SF128A", SA_64KB, TOTAL_SIZE_16MB}
};

const FlashDeviceConfig issDevices[] __attribute__((section(".rodata"))) = {
    {DEVICE_ID_IS25WP256, "IS25WP256", SA_4KB, TOTAL_SIZE_32MB}
};

// Lookup table for all manufacturers
const Manufacturer supportedManufacturers[] __attribute__((section(".rodata"))) = {
    {CYPRESS_MANUFACTURER_ID,  "Cypress",  cypressDevices,  sizeof(cypressDevices) / sizeof(cypressDevices[0])},
    {WINBOND_MANUFACTURER_ID,  "Winbond",  winbondDevices,  sizeof(winbondDevices) / sizeof(winbondDevices[0])},
    {MACRONIX_MANUFACTURER_ID, "Macronix", macronixDevices, sizeof(macronixDevices) / sizeof(macronixDevices[0])},
    {MICRON_MANUFACTURER_ID,   "Micron",   micronDevices,   sizeof(micronDevices) / sizeof(micronDevices[0])},
    {RENESAS_MANUFACTURER_ID,   "Renesas",   renesasDevices,   sizeof(renesasDevices) / sizeof(renesasDevices[0])},
    {ISS_MANUFACTURER_ID,      "ISS",      issDevices,      sizeof(issDevices) / sizeof(issDevices[0])}
};
const size_t supportedManufacturerCount = sizeof(supportedManufacturers) / sizeof(supportedManufacturers[0]);

// Finds a flash device configuration based on manufacturer and device IDs.
static const FlashDeviceConfig* findFlashDevice(uint8_t manufacturerId, uint16_t deviceId) {
    for (size_t i = 0; i < supportedManufacturerCount; i++) {
        if (supportedManufacturers[i].manufacturerId == manufacturerId) {
            const Manufacturer* m = &supportedManufacturers[i];
            for (size_t j = 0; j < m->deviceCount; j++) {
                if (m->devices[j].deviceId == deviceId) {
                    return &m->devices[j];
                }
            }
            return NULL;
        }
    }
    return NULL;
}

static int32_t checkQspiFlashDevice(void) {
    uint32_t readDevId;
    uint8_t manufacturerId;
    uint16_t deviceId;

    ReadQspiFlashID(&readDevId);

    manufacturerId = (uint8_t)(readDevId & 0x000000FF);
    deviceId = (uint16_t)((readDevId & 0x0000FF00) | ((readDevId >> 16) & 0x000000FF));

    const FlashDeviceConfig* config = findFlashDevice(manufacturerId, deviceId);

    if (config) {
        gFlashDevice.sectorSize = config->sectorSize;
        gFlashDevice.endAddress = config->totalSize - 0x8000 - 1;
        gFlashDevice.deviceId = deviceId;
        gFlashDevice.manufacturerId = manufacturerId;
        return 0;
    }

    char str[64];
    Data2HexAscii(deviceId, str, 4);
    PutStr("Unsupported FlashID = 0x", 0);
    PutStr(str, 1);

    return 1;
}

static bool setAddressInput(uint32_t* address) {
    PutStr("Send . to cancel", 1);
    if (!getUserInput(address, "Please Input : H'")) {
        return false;
    }
    if (*address & 0x3) {
        PutStr("Memory Boundary Error", 1);
        return false;
    }
    return true;
}

static bool setSizeInput(uint32_t* size) {
    PutStr("Send . to cancel", 1);
    return getUserInput(size, "Please Input : H'");
}

static void initializeFlashLoad(void) {
    PutStr("===== SPI writing of RZ Board Command =============", 1);
    PutStr("Load Program to Spiflash", 1);
    PutStr("Writes to any of SPI address.", 1);
    Init_SPIFlash();
}

static bool getProgramParameters(LoadFormat format, uint32_t* prgSpiStartAdd, uint32_t* prgSpiSize, uint32_t* userPrgStartAdd) {
    PutStr(format == LOAD_SREC ? "Program Top Address & Qspi Save Address " : "Program size & Qspi Save Address ", 1);

    *prgSpiStartAdd = 0;
    *prgSpiSize = 0;

    if (format == LOAD_SREC) {
        PutStr("===== Please Input Program Top Address ============", 1);
        if (!setAddressInput(userPrgStartAdd)) return false;
    } else {
        PutStr("===== Please Input Program size ============", 1);
        if (!setSizeInput(prgSpiSize)) return false;
    }

    PutStr(" ", 1);
    PutStr("===== Please Input Qspi Save Address ===", 1);
    if (!setAddressInput(prgSpiStartAdd)) return false;

    if (gFlashDevice.endAddress < *prgSpiStartAdd) {
        PutStr("Address Input Error", 1);
        PutStr("Range of H'000_0000 ~ H'0FF_7FFF", 1);
        return false;
    }
    return true;
}

static bool loadBinaryData(uint32_t* workAddMin, uint32_t* workAddMax, uint32_t prgSpiSize) {
    char binData;
    uint32_t imageOffset = 0;
    uintptr_t loadWorkStartAdd = LS_WORK_MEM_SADD;

    PutStr("Send . to stop", 1);
    PutStr("please send ! (binary)", 1);

    while (imageOffset < prgSpiSize) {
        GetChar(&binData);
        // Check for cancellation sequence ".<CR>"
        if (imageOffset == 0 && binData == '.') {
            char nextChar;
            GetChar(&nextChar);
            if (nextChar == CR_CODE) {
                return false;
            }
            *(uint8_t*)(loadWorkStartAdd + imageOffset++) = binData;
            if (imageOffset < prgSpiSize) {
                *(uint8_t*)(loadWorkStartAdd + imageOffset++) = nextChar;
            }
        } else {
            *(uint8_t*)(loadWorkStartAdd + imageOffset++) = binData;
        }
    }
    *workAddMin = loadWorkStartAdd;
    *workAddMax = loadWorkStartAdd + prgSpiSize - 1;

    return true;
}

static void skipToEndOfLine(void) {
    char str;
    do {
        GetChar(&str);
    } while (str != CR_CODE && str != LF_CODE);
}

static bool readRecordType(char typeChar, uint32_t* adByteCount, bool* endFlg) {
    switch (typeChar) {
        case '0': return false; // S0 record, skip
        case '1': *adByteCount = 2; return true;
        case '2': *adByteCount = 3; return true;
        case '3': *adByteCount = 4; return true;
        case '7':
        case '8':
        case '9': *endFlg = true; return false; // End records
        default:  *endFlg = true; return false; // Invalid type, treat as end
    }
}

static bool processRecordPayload(uint32_t payloadSize, uint8_t** ptr, uint32_t* maxAdd) {
    char str[3];
    uint32_t data;
    str[2] = '\0';

    for (uint32_t i = 0; i < payloadSize; i++) {
        GetStr_ByteCount(str, 1);
        if (HexAscii2Data((unsigned char*)str, &data)) {
            return false;
        }
        **ptr = (uint8_t)data;
        (*ptr)++;
        (*maxAdd)++;
    }
    return true;
}

static bool processSrecRecord(uint8_t** ptr, uint32_t* maxAdd, bool* endFlg) {
    char str[12];
    uint32_t adByteCount, payloadSize, address;

    GetChar(str);
    if (!readRecordType(str[0], &adByteCount, endFlg)) {
        return *endFlg ? false : true; // Stop on end flag, continue on S0
    }

    // Read byte count
    GetStr_ByteCount(str, 1);
    if (HexAscii2Data((unsigned char*)str, &payloadSize)) return false;

    // Read address
    GetStr_ByteCount(str, adByteCount);
    if (HexAscii2Data((unsigned char*)str, &address)) return false;

    // Process data payload
    uint32_t dataSize = payloadSize - adByteCount - 1; // -1 for checksum
    if (!processRecordPayload(dataSize, ptr, maxAdd)) return false;

    // Read and discard checksum and CR/LF
    GetStr_ByteCount(str, 1);
    skipToEndOfLine();

    return true;
}

static bool loadSrecData(uint32_t* maxAdd, uint32_t* minAdd) {
    char str;
    uint8_t* ptr = (uint8_t*)LS_WORK_MEM_SADD;
    *minAdd = LS_WORK_MEM_SADD;
    *maxAdd = LS_WORK_MEM_SADD;
    bool endFlg = false;

    PutStr("Send . to stop", 1);
    PutStr("please send ! ('.' & CR stop load)", 1);

    while (!endFlg) {
        // Wait for start of a record ('S' or 's') or cancellation ('.')
        do {
            GetChar(&str);
            if (str == '.') {
                skipToEndOfLine();
                return false; // User cancelled
            }
        } while (str != 's' && str != 'S');

        if (!processSrecRecord(&ptr, maxAdd, &endFlg)) {
            if (endFlg) break; // Error or end record found
            // S0 record was skipped, continue loop
        }
    }

    *maxAdd -= 1;
    return true;
}

static bool validateProgramSize(uint32_t prgSpiStartAdd, uint32_t prgSpiEndAdd) {
    char str[64];
    if (prgSpiEndAdd > gFlashDevice.endAddress) {
        PutStr("Program over size Error", 1);
        PutStr(" SpiFlashMemory Stat Address : H'", 0);
        Data2HexAscii(prgSpiStartAdd, str, 4);
        PutStr(str, 1);
        PutStr(" SpiFlashMemory End Address  : H'", 0);
        Data2HexAscii(prgSpiEndAdd, str, 4);
        PutStr(str, 1);
        return false;
    }
    return true;
}

static void loadSpiflash(LoadFormat format) {
    uint32_t prgSpiStartAdd, prgSpiSize;
    uint32_t userPrgStartAdd;
    uint32_t workAddMin, workAddMax;
    uintptr_t loadWorkStartAdd = LS_WORK_MEM_SADD;

    initializeFlashLoad();
    if (checkQspiFlashDevice() != 0) {
        return;
    }

    if (!getProgramParameters(format, &prgSpiStartAdd, &prgSpiSize, &userPrgStartAdd)) {
        return;
    }

    bool loadSuccess = false;
    if (format == LOAD_SREC) {
        loadSuccess = loadSrecData(&workAddMax, &workAddMin);
    } else {
        loadSuccess = loadBinaryData(&workAddMin, &workAddMax, prgSpiSize);
    }

    if (!loadSuccess) {
        return;
    }

    prgSpiStartAdd += (workAddMin - loadWorkStartAdd);
    if (!validateProgramSize(prgSpiStartAdd, prgSpiStartAdd + (workAddMax - workAddMin))) {
        return;
    }

    writeToFlash(prgSpiStartAdd, workAddMin, workAddMax);
}

/********************************************************
    MODULE          : dgClearSpiflash0
    FUNCTION        : Clear Spiflash memory
    COMMAND         : XCS
    INPUT PARAMETER : N/A
*********************************************************/
void dgClearSpiflash0(void) {
    PutStr("ALL ERASE SpiFlash memory ", 1);
    PutStr("Clear OK?(y/n)", 0);

    if (WaitKeyIn_YorN()) {
        DelStr(14);
        return;
    }

    DelStr(14);
    Init_SPIFlash();

    if (checkQspiFlashDevice() != 0) {
        return;
    }

    PutStr("ERASE QSPI-FLASH (60sec[typ])....", 0);
    ChipEraseQspiFlash();
    PutStr("complete!", 1);
}

/************************************************************************
    MODULE          : dgLoadSpiFlashSrec
    FUNCTION        : load Program to Spiflash memory (srec/mot)
    COMMAND         : XLS2
    INPUT PARAMETER : N/A
*************************************************************************/
void dgLoadSpiFlashSrec(void) {
    loadSpiflash(LOAD_SREC);
}

/************************************************************************
    MODULE          : dgLoadSpiFlashBinary
    FUNCTION        : load Program to Spiflash memory (bin)
    COMMAND         : XLS3
    INPUT PARAMETER : N/A
*************************************************************************/
void dgLoadSpiFlashBinary(void) {
    loadSpiflash(LOAD_BINARY);
}
