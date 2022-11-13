#include <stdio.h>
#include <stdint.h>

#include "recTypes.h"
#include "fileOps.h"

uint8_t searchForParser(uint8_t val){

    for (unsigned long i = 0; i < sizeof(recordTypes)/sizeof(recordType); i++) {
        if ( recordTypes[i].recType == val ) {
            printf("Record description:\t%s\n", recordTypes[i].recordName);
            return (uint8_t)i;
        }
    }
    printf("Error:\tRecordType %02X not parsed\n", val);
    return parseErrType;
}

uint8_t parserErr(uint16_t len) {
    printf("Data:\t");
    for (uint16_t i = 0; i < len; i++) {
        uint8_t data;
        data = fileReadByte();
        printf("%02X ", data);
    }
    printf("\n");
    return 0;
}

uint8_t parserRHEADR(uint16_t len) {
    uint8_t nameLen;
    uint8_t data;
    nameLen = fileReadByte();
    if ( (nameLen > 0) && (nameLen <= 40) ) {
        printf("Module name:\t");
        for (uint8_t i = 0; i < nameLen; i++) {
            data = fileReadByte();
            printf("%c", data);
        }
        printf("\n");
    }
    
    uint8_t modAttrData;
    modAttrData = fileReadByte();
    printf("Module type:\t%02X\t", modAttrData);
    switch (modAttrData) {
        case 0:
            printf("Absolute module.\n");
            break;
        case 1:
            printf("Reoloctable module. Fixups  other than base fixuls may still be present.\n");
            break;
        case 2:
            printf("Position Independent Code module. It can be loaded anywhere. No fixups are needed.\n");
            break;
        case 3:
            printf("Load-Time Locatable module. It can be loaded anywhere with perhaps some base fixups to be performed.\n");
            break;
        default:
            printf("Error: Other attribute type. Stop parsing record\n");
            return 0;
            break;

    }
    
    uint16_t segCount = fileReadWord();
    printf("Segment record count:\t%u\t%02X\n", segCount, segCount);

    uint16_t grpCount = fileReadWord();
    printf("Group record count:\t%u\t%02X\n", grpCount, grpCount);

    uint16_t ovlCount = fileReadWord();
    printf("Overlay record count:\t%u\t%02X\n", ovlCount, ovlCount);

    uint32_t ovlOffset = fileReadLong();
    printf("Overlay Offset:\t%u\t0x%04X\n", ovlOffset, ovlOffset);

    uint32_t staticSize = fileReadLong();
    printf("Static Size:\t%u\t0x%04X\n", staticSize, staticSize);

    uint32_t maxStaticSize = fileReadLong();
    printf("Maximums Static Size:\t%u\t0x%04X\n", maxStaticSize, maxStaticSize);

    uint32_t dynSize = fileReadLong();
    printf("Dynamic Size:\t%u\t0x%04X\n", dynSize, dynSize);

    uint32_t maxDynSize = fileReadLong();
    printf("Maximum dynamic Size:\t%u\t0x%04X\n", maxDynSize, maxDynSize);

    return 0;
};

uint8_t parserREGINT(uint16_t len) {
    uint8_t regType;
    regType  = fileReadByte();
    printf("Regyster type (RAW):\t0x%02X\n", regType);

    uint8_t regPair;
    regPair = (regType & 0b11000000) >> 6;
    bool regTypeLogical;
    regTypeLogical = (regType & 0b00000001);

    printf("Init registers: (type=%u)\t", regPair);
    switch (regPair) {
        case 0:
            printf("CS:IP\n");
            break;
        case 1:
            printf("SS:SP\n");
            break;
        case 2:
            printf("DS\n");
            break;
        case 3:
            printf("ES\n");
            break;
        default:
            printf("Error: Other/Unknown\n");
            break;

    }

    if ( regTypeLogical == 1 ) {
        printf("Init registers as logical with fixup (L=1)\n");
    } else {
        printf("Init registers as base:offset pair (L=0)\n");
    }

    printf("Data:\t");
    for (uint16_t i = 0; i < len - 1; i++) {
        uint8_t data;
        data = fileReadByte();
        printf("%02X ", data);
    }
    printf("\n");
    printf("Warning: partial parsing. Need more codding.\n");
    return 0;
}

uint8_t parserMODEND(uint16_t len) {
    uint8_t modType;
    modType = fileReadByte();
    printf("Module type (RAW):\t0x%02X\n", modType);
    
    uint8_t modAttribute;
    modAttribute = (modType & 0b11000000) >> 6;
    bool modTypeLogical;
    modTypeLogical = (modType & 0b00000001);
    
    printf("Init registers: (type=%u)\t", modAttribute);
    switch (modAttribute) {
        case 0:
            printf("Non-main module with NO Start Address\n");
            break;
        case 1:
            printf("Non-main module WITH Start Address\n");
            break;
        case 2:
            printf("Main module with NO Start Address\n");
            break;
        case 3:
            printf("Main module WITH Start Address\n");
            break;
        default:
            printf("Error: Other/Unknown module attribute:\t%u\n", modAttribute);
            break;
    }

    if ( modAttribute == 1  || modAttribute == 3 ) {
        if ( modTypeLogical == 1 ) {
            printf("Start Address is logical, requre fixup (L=1)\n");
        } else {
            printf("Start Address is physical CS:IP pair (L=0)\n");
        }

        printf("Data:\t");
        for (uint16_t i = 0; i < len-1; i++) {
            uint8_t data;
            data = fileReadByte();
            printf("%02X ", data);
        }
        printf("\n");
        printf("Warning: parsing not implemented yet. Need more codding.\n");
    } else {
        printf("Note:\tL=%u, does not matter when no Start Address.\n", modTypeLogical);
    }
    return 0;
}
uint8_t parserTHEADR(uint16_t len) {
    uint8_t nameLen;
    uint8_t data;
    nameLen = fileReadByte();
    //if ( (nameLen > 0) && (nameLen <= 40) ) {
        printf("Module name:\t");
        for (uint8_t i = 0; i < nameLen; i++) {
            data = fileReadByte();
            printf("%c", data);
        }
        printf("\n");
    //}
    return 0;
}

uint8_t parserCOMENT(uint16_t len) {
    //uint8_t nameLen;
    uint8_t data;
    uint16_t commentType;
    commentType = fileReadWord();
    
    printf("Comment type (RAW):\t0x%04X\n", commentType);
    if ( commentType & 0x8000 ) {
        printf("NOPURGE Flag is set\n");
    }
    if ( commentType & 0x4000 ) {
        printf(" NOLIST Flag is set\n");
    }

    printf("Comment class:\t0x%02X\n", commentType & 0xff);
    for (uint8_t i = 0; i < (len - 2); i++) {
        data = fileReadByte();
        printf("%c", data);
        //printf("%02X ", data);
    }
    printf("\n");
    return 0;
}

uint8_t parserSEGDEF(uint16_t len) {
    uint8_t data;
    uint8_t aligment;
    uint8_t combination;
    bool big;
    bool pageResident;
    aligment = fileReadByte();
    printf("ACBP RAW: 0x%02X\n", aligment);
    combination = (aligment & 0b00011100) >> 2;
    big = aligment & 0b00000010 ? true : false;
    pageResident = aligment & 0b00000001 ? true : false;
    aligment = (aligment & 0b11100000) >> 5;

    printf("Aligment: %u\nType: ", aligment);
        switch (aligment) {
        case 0:
            printf("SEGDEF describes an absolute LSEG\n");
            break;
        case 1:
            printf("SEGDEF describes a relocatable, byte aligned LSEG\n");
            break;
        case 2:
            printf("SEGDEF describes a relocatable, word aligned LSEG\n");
            break;
        case 3:
            printf("SEGDEF describes a relocatable, paragraph aligned LSEG\n");
            break;
        case 4:
            printf("SEGDEF describes a relocatable, page aligned LSEG\n");
            break;
        case 5:
            printf("SEGDEF describes an unnamed absolute portion of memory\n");
            break;
        case 6:
            printf("SEGDEF describes a load-time locatable (LTL), paragraph aligned LSEF if not member of any group\n");
            break;
        default:
            printf("Error: Other/Unknown Aligment:\t%u\n", aligment);
            break;
    }
    printf("Combination: %u\n", combination);
    if (big) {
        printf("Flag: BIG. Segment lenght is 65536\n");
    }
    if (pageResident) {
        printf("Flag: pageResident. Segment located corssing page boundary\n");
    }

    printf("Not parsed data (RAW):\n");
    for (uint8_t i = 0; i < (len - 1); i++) {
        data = fileReadByte();
        //printf("%c", data);
        printf("%02X ", data);
    }
    printf("\n");
    printf("Warning: parsing not implemented yet. Need more codding.\n");
    return 0;
}
//uint8_t parserLEDATA (uint16_t len) {
//    
//    return 0;
//}
