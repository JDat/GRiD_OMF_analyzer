#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "recTypes.h"
#include "fileOps.h"

uint8_t searchForParser(uint8_t val){

    for (unsigned long i = 0; i < sizeof(recordTypes)/sizeof(recordType); i++) {
        if ( recordTypes[i].recType == val ) {
            printf("Record description:\t%s\n", recordTypes[i].recordName);
            return (uint8_t)i;
        }
    }
    printf("Warning:\tRecordType %02X not parsed\n", val);
    return parseErrType;
}

uint8_t parserErr(uint16_t len) {
    printf("Data:\t");
    while (len) {
        uint8_t data;
        data = fileReadByte();
        len--;
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
    //uint8_t printCount = 0;
    uint8_t data;
    
    nameLen = fileReadByte();
    //if ( (nameLen > 0) && (nameLen <= 40) ) {
        printf("Module name:\t");
        
        for (uint8_t i = 0; i < nameLen; i++) {
            data = fileReadByte();
            printf("%c", data);
        }
        
        /*
        for (uint8_t i = 0; i < (len ); i++) {
            data = fileReadByte();
            
            if (data > 1 && printCount == 0) {
                printCount = data + 1 ;
                printf("\nASCII data %u chars len:  ", data);
            }

            if (printCount > 0 ) {
                printf("%c", data);
                printCount--;
            } else {
                printf("%02X ", data);
            }

        }
        */
        printf("\n");
    //}
    return 0;
}

uint8_t parserLHEADR(uint16_t len) {
    uint8_t nameLen;
    //uint8_t printCount = 0;
    uint8_t data;
    
    nameLen = fileReadByte();
    //if ( (nameLen > 0) && (nameLen <= 40) ) {
        printf("Module name:\t");
        
        for (uint8_t i = 0; i < nameLen; i++) {
            data = fileReadByte();
            printf("%c", data);
        }
        
        /*
        for (uint8_t i = 0; i < (len ); i++) {
            data = fileReadByte();
            
            if (data > 1 && printCount == 0) {
                printCount = data + 1 ;
                printf("\nASCII data %u chars len:  ", data);
            }

            if (printCount > 0 ) {
                printf("%c", data);
                printCount--;
            } else {
                printf("%02X ", data);
            }

        }
        */
        printf("\n");
    //}
    return 0;
}

uint8_t parserCOMENT(uint16_t len) {
    //uint8_t nameLen;
    uint8_t printCount = 0;
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
        
        if (data > 1 && printCount == 0) {
            printCount = data + 1 ;
            printf("\nASCII data %u chars len:  ", data);
        }

        if (printCount > 0 ) {
            printf("%c", data);
            printCount--;
        } else {
            printf("%02X ", data);
        }

    }
    printf("\n");
    return 0;
}

uint8_t parserSEGDEF(uint16_t len) {
    uint8_t data;
    uint8_t aligment;
    uint8_t combination;
        
    uint8_t ltlData;
    
    uint16_t maxSegLen;
    uint16_t groupOffset;
    
    uint16_t segLen;
    
    uint16_t segmentNameIndex;
    uint16_t groupNameIndex;
    uint16_t overlayNameIndex;
    
    bool flagBig;
    bool flagPageResident;

    bool flagGroup;
    bool flagBSM;
    
    aligment = fileReadByte();
    len--;
    printf("ACBP RAW: 0x%02X\n", aligment);
    combination = (aligment & 0b00011100) >> 2;
    flagBig = aligment & 0b00000010 ? true : false;
    flagPageResident = aligment & 0b00000001 ? true : false;
    aligment = (aligment & 0b11100000) >> 5;

    printf("Aligment: %u\nType: ", aligment);
        switch (aligment) {
        case 0:
            printf("SEGDEF describes an absolute LSEG\n");
            break;
        case 1:
            printf("SEGDEF describes a relocatable, byte aligned LSEG\n");
            printf("Warning: Combination not implemented yet!\n");
            break;
        case 2:
            printf("SEGDEF describes a relocatable, word aligned LSEG\n");
            printf("Warning: Combination not implemented yet!\n");
            break;
        case 3:
            printf("SEGDEF describes a relocatable, paragraph aligned LSEG\n");
            printf("Warning: Combination not implemented yet!\n");
            break;
        case 4:
            printf("SEGDEF describes a relocatable, page aligned LSEG\n");
            printf("Warning: Combination not implemented yet!\n");
            break;
        case 5:
            printf("SEGDEF describes an unnamed absolute portion of memory\n");
            break;
        case 6:
            printf("SEGDEF describes a load-time locatable (LTL), paragraph aligned LSEG if not member of any group\n");
            ltlData = fileReadByte();
            len--;
            flagGroup = ltlData & 0b10000000 ? true : false;
            flagBSM = ltlData & 0b00000001 ? true : false;
            maxSegLen = fileReadWord();
            groupOffset = fileReadWord();
            len -= 4;
            segLen = fileReadWord();
            len -= 2;

            data = fileReadByte();
            len--;
            if (data > 127) {
                segmentNameIndex = data & 0x7F << 8;
                data = fileReadByte();
                len--;
                segmentNameIndex += data;
            } else {
                segmentNameIndex = data;
            }

            data = fileReadByte();
            len--;
            if (data > 127) {
                groupNameIndex = data & 0x7F << 8;
                data = fileReadByte();
                len--;
                groupNameIndex += data;
            } else {
                groupNameIndex = data;
            }

            data = fileReadByte();
            len--;
            if (data > 127) {
                overlayNameIndex = data & 0x7F << 8;
                data = fileReadByte();
                len--;
                overlayNameIndex += data;
            } else {
                overlayNameIndex = data;
            }

            break;
        default:
            printf("Error: Other/Unknown Aligment:\t%u\n", aligment);
            break;
    }
    printf("Combination: %u\n", combination);
    if (flagBig) {
        printf("Flag: BIG. Segment length is 65536.\n");
    }
    if (flagPageResident) {
        printf("Flag: pageResident. Segment located corssing page boundary\n");
    }

    if (aligment == 6 ) {
        printf("LTL Data (RAW): 0x%02X\n", ltlData);
        if (flagGroup) {
            printf("Flag: GROUP. Segment is part of group and should be loaded as part of the group\n");
        }
        if (flagBSM) {
            printf("Flag: BSM. Maximum segment length is 65536. MAXIMUM SEGMENT LENGTH must be 0\n");
        }

        printf("Max Segment Length:\t %u\t0x%04X\n", maxSegLen, maxSegLen);
        printf("Group offset:\t %u\t0x%04X\n", groupOffset, groupOffset);

        printf("Segment Length:\t %u\t0x%04X\n", segLen, segLen);
        
        printf("Segment Name Index:\t %u\t0x%04X\n", segmentNameIndex, segmentNameIndex);
        printf("Group Name Index:\t %u\t0x%04X\n", groupNameIndex, groupNameIndex);
        printf("Overlay Name Index:\t %u\t0x%04X\n", overlayNameIndex, overlayNameIndex);

    }
    
    
    if (len) {
        printf("Data: ");
        while (len) {
            data = fileReadByte();
            len--;
            //printf("%c", data);
            printf("%02X ", data);
        }
        printf("\n");
        printf("Warning: parsing not implemented yet. Need more codding\n");
    }
    return 0;
}

uint8_t parserLNAMES(uint16_t len) {
    uint16_t i = 0;
    uint8_t data;
    uint8_t strLen = 0;
    uint8_t entryCount = 0;
    
    //char tmpStr[40];
    
    while (i < len) {
        i++;
        data = fileReadByte();
        printf("Index: %u:\t", entryCount);
        entryCount++;
        
        if (data > 0) {
            strLen = data;
        } else {
            printf("NULL entry");
        }
        
        while (strLen) {
            strLen--;
            i++;
            data = fileReadByte();
            printf("%c", data);
        }
        printf("\n");
        //printf("tmpStr: %s\n", tmpStr);
        
    }
    
    printf("Entry count: %u\n", entryCount);
    return 0;
}

uint8_t parserTYPDEF(uint16_t len) {
    uint8_t data;
    
    data = fileReadByte();
    len--;
    if (data) {
        printf("Warning: Name must be 0x00, but we have 0x%02X. This is woring!\n", data);
    }
    
    while (len) {
        data = fileReadByte();
        len--;
        switch (data) {
            case 0 ... 127:
                printf("Empty leaf %u\n", data);
                break;
            case 128:
                printf("NULL leaf %u\n", data);
                break;
            default:
                printf("Warning: leaf parsing not implemented!. RAW value: 0x%02X\n", data);
                break;
        }
    }
    return 0;
}

uint8_t parserLEDATA(uint16_t len) {
    uint8_t data;
    uint16_t segmentIndex;
    uint16_t enumeratedDataOffset;
    
    data = fileReadByte();
    len--;
    if (data > 127) {
        segmentIndex = data & 0x7F << 8;
        data = fileReadByte();
        len--;
        segmentIndex += data;
    } else {
        segmentIndex = data;
    }
    printf("Segment index:\t%u\t0x%04X\n", segmentIndex, segmentIndex);
    enumeratedDataOffset = fileReadWord();
    len -=2;
    printf("Enumerated Data Offset:\t%u\t0x%04X\n", enumeratedDataOffset, enumeratedDataOffset);
    
    printf("Data size:\t%u\t0x%04X\n", len, len);
    printf("Data:\t");
    while (len) {
        uint8_t data;
        data = fileReadByte();
        len--;
        printf("%02X ", data);
    }
    printf("\n");
    
    return 0;
}

uint8_t parserGRPDEF(uint16_t len) {
    uint8_t data;
    uint16_t groupNameIndex;
    uint8_t groupComponentDescriptor;
    uint16_t segmentIndex;
    
    data = fileReadByte();
    len--;
    if (data > 127) {
        groupNameIndex = data & 0x7F << 8;
        data = fileReadByte();
        len--;
        groupNameIndex += data;
    } else {
        groupNameIndex = data;
    }
    printf("Group Name Index:\t%u\t0x%04X\n", groupNameIndex, groupNameIndex);
    
    while (len) {
        groupComponentDescriptor = fileReadByte();
        len--;
        printf("Entry:\n");
        switch (groupComponentDescriptor) {
            case groupSI:
                data = fileReadByte();
                len--;
                if (data > 127) {
                    segmentIndex = data & 0x7F << 8;
                    data = fileReadByte();
                    len--;
                    segmentIndex += data;
                } else {
                    segmentIndex = data;
                }
                printf("Group Component Descriptor Field For Segment Index:\t0x%04X\n", segmentIndex);
                break;
/*
            case groupEI:
                break;
            case groupSCO:
                break;
*/
            case groupLTL:
                uint8_t ltlData;
                bool ltlBGL;
                bool ltlBGM;
                uint16_t maximumGroupLength;
                uint16_t groupLength;
                
                ltlData = fileReadByte();
                len--;
                ltlBGL = ltlData & 0x00000010 ? true : false;
                ltlBGM = ltlData & 0x00000001 ? true : false;
                
                maximumGroupLength = fileReadWord();
                groupLength = fileReadWord();
                len -=4;
                printf("Group Component Descriptor Field For LTL data. RAW:\t0x%02X\n", ltlData);
                if (ltlBGL) {
                    printf("BGL Is Set. Group Len Is 64k\n");
                    if (groupLength) {
                        printf("Error: Group Length Must Be 0!\n");
                        printf("Error: Group Length:\t%u\t0x%04X\n", groupLength, groupLength);
                    }
                }
                if (ltlBGM) {
                    printf("BGM Is Set. Max Group Len Is 64k\n");
                    if (maximumGroupLength) {
                        printf("Error: Maximum Group Length Must Be 0!\n");
                        printf("Error: Maximum Length:\t%u\t0x%04X\n", maximumGroupLength, maximumGroupLength);
                    }
                }
                
                printf("Group Length:\t%u\t0x%04X\n", groupLength, groupLength);
                printf("Maximum Group Length:\t%u\t0x%04X\n", maximumGroupLength, maximumGroupLength);
                //printf("\n");
                break;
/*
            case groupABS:
                break;
*/
            default:
                printf("Error: Unknown Group component descriptor 0x%02X\n", groupComponentDescriptor);
                printf("Error: Dumping remaining data in record: ");
                while (len) {
                    data = fileReadByte();
                    len--;
                    printf("%02X ", data);
                }
                printf("\n");
                break;
        }
    }
    return 0;
}

uint8_t parserREDATA(uint16_t len) {
    uint8_t data;
    
    uint16_t groupIndex;
    uint16_t segmentIndex;
    uint16_t frameIndex;
    uint16_t dataRecordOffset;
    
    data = fileReadByte();
    len--;
    if (data > 127) {
        groupIndex = data & 0x7F << 8;
        data = fileReadByte();
        len--;
        groupIndex += data;
    } else {
        groupIndex = data;
    }
    
    data = fileReadByte();
    len--;
    if (data > 127) {
        segmentIndex = data & 0x7F << 8;
        data = fileReadByte();
        len--;
        segmentIndex += data;
    } else {
        segmentIndex = data;
    }
    
    printf("Group Index:\t%u\t0x%04X\n", groupIndex, groupIndex);
    printf("Segment Index:\t%u\t0x%04X\n", segmentIndex, segmentIndex);
    
    if (groupIndex) {
        if (segmentIndex) {
            //noframeIndex, normal
        } else {
            printf("Warning: Segment Index=0, Must be nonZero!\n");
        }
    } else {    // groupIndex = 0
        if (segmentIndex) {
            //noframeIndex, normal
        } else {
            data = fileReadByte();
            len--;
            if (data > 127) {
                frameIndex = data & 0x7F << 8;
                data = fileReadByte();
                len--;
                frameIndex += data;
            } else {
                frameIndex = data;
            }
            printf("Frame Index:\t%u\t0x%04X\n", frameIndex, frameIndex);
        }
    }
    
    dataRecordOffset = fileReadWord();
    len -= 2;
    printf("Data Record Offset:\t%u\t0x%04X\n", dataRecordOffset, dataRecordOffset);

    printf("Data:\t");
    while (len) {
        uint8_t data;
        data = fileReadByte();
        len--;
        printf("%02X ", data);
    }
    printf("\n");
    
    return 0;
}
