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

    if (ovlCount == 0 && ovlOffset != 0 ) {
        printf("Warning: When Overlay record count = 0, Overlay Offset must be = 0\n");
    }
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
    uint8_t data;

    uint8_t regPair;    
    uint8_t regType;
    uint16_t groupIndex;
    uint16_t segmentIndex;
    uint16_t frameIndex;
    uint16_t registerOffset;

    uint16_t entryCount;
    
    while(len) {
        printf("\n");
        printf("Entry:\t%u\t0x%04X\n", entryCount, entryCount);
        entryCount++;
        regType  = fileReadByte();
        len--; 
        
        printf("Regyster type (RAW):\t0x%02X\n", regType);


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
            printf("Warning: Fixup not implemented yet. Need more codding. Dumping remaining data: ");
            while(len) {
                uint8_t data;
                data = fileReadByte();
                len--;
                printf("%02X ", data);
            }
        } else {
            printf("Init registers as base:offset pair (L=0)\n");
                    
            data = fileReadByte();
            len--;
            if (data > 127) {
                groupIndex = (data & 0x7F) << 8;
                data = fileReadByte();
                len--;
                groupIndex += data;
            } else {
                groupIndex = data;
            }
            
            data = fileReadByte();
            len--;
            if (data > 127) {
                segmentIndex = (data & 0x7F) << 8;
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
            
            if (regPair <= 1) {
                registerOffset = fileReadWord();
                len -= 2;
                if (segmentIndex) {
                    printf("Register Offset Relative To Segment Index:\t%u\t0x%04X\n", segmentIndex, segmentIndex);
                } else {
                    printf("Register Offset Relative To Frame Index:\t%u\t0x%04X\n", frameIndex, frameIndex);
                }
                printf("Register Offset:\t%u\t0x%04X\n", registerOffset, registerOffset);
            }
        
        }
    }
    
    printf("\n");
    printf("Total entries:\t%u\t0x%04X\n", entryCount, entryCount);
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

bool isPrintable(uint8_t data) {
    if ( (data >= 0x20) && (data < 0x7f) ) { 
        return true;
    } else {
        return false;
    }
}
uint8_t parserCOMENT(uint16_t len) {
    uint8_t data;
    uint8_t oldData = 0;
    uint16_t commentType;
    commentType = fileReadWord();
    len -= 2;
    
    printf("Comment type (RAW):\t0x%04X\n", commentType);
    if ( commentType & 0x8000 ) {
        printf("NOPURGE Flag is set\n");
    }
    if ( commentType & 0x4000 ) {
        printf(" NOLIST Flag is set\n");
    }

    printf("Comment class:\t0x%02X\n", commentType & 0xff);
    
    if (len) {
        while (len) {
            data = fileReadByte();
            len--;
            if (isPrintable(oldData) && !isPrintable(data) ) { 
                printf("\n");
            }
            if (!isPrintable(oldData) && isPrintable(data) ) { 
                printf("\n");
            }
            
            if (isPrintable(data)){
                printf("%c", data);
            } else {
                printf("%02X ", data);
            }
            oldData = data;
        }
    }
    printf("\n\n");
    
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
    
    uint16_t frameNumber;
    uint8_t offset;
    
    bool flagBig;
    bool flagPageResident;

    bool flagGroup;
    bool flagBSM;
    
    aligment = fileReadByte();
    len--;
    printf("ACBP RAW: 0x%02X\n", aligment);
    combination = (aligment & 0b00011100) >> 2;
    flagBig = (aligment & 0b00000010) ? true : false;
    flagPageResident = (aligment & 0b00000001) ? true : false;
    aligment = (aligment & 0b11100000) >> 5;

    printf("Aligment: %u\nType: ", aligment);
    switch (aligment) {
        case 0:
            printf("SEGDEF describes an absolute LSEG\n");
            break;
        case 1:
            printf("SEGDEF describes a relocatable, byte aligned LSEG\n");
            printf("Warning: Aligment type not implemented yet!\n");
            break;
        case 2:
            printf("SEGDEF describes a relocatable, word aligned LSEG\n");
            //printf("Warning: Aligment type not implemented yet!\n");
            segLen = fileReadWord();
            len -= 2;
            
            data = fileReadByte();
            len--;
            if (data > 127) {
                segmentNameIndex = (data & 0x7F) << 8;
                data = fileReadByte();
                len--;
                segmentNameIndex += data;
            } else {
                segmentNameIndex = data;
            }

            data = fileReadByte();
            len--;
            if (data > 127) {
                groupNameIndex = (data & 0x7F) << 8;
                data = fileReadByte();
                len--;
                groupNameIndex += data;
            } else {
                groupNameIndex = data;
            }

            data = fileReadByte();
            len--;
            if (data > 127) {
                overlayNameIndex = (data & 0x7F) << 8;
                data = fileReadByte();
                len--;
                overlayNameIndex += data;
            } else {
                overlayNameIndex = data;
            }
            break;
        case 3:
            printf("SEGDEF describes a relocatable, paragraph aligned LSEG\n");
            printf("Warning: Aligment type not implemented yet!\n");
            break;
        case 4:
            printf("SEGDEF describes a relocatable, page aligned LSEG\n");
            printf("Warning: Aligment type not implemented yet!\n");
            break;
        case 5:
            printf("SEGDEF describes an unnamed absolute portion of memory\n");
            frameNumber = fileReadWord();
            offset = fileReadByte();
            ltlData = fileReadByte();
            len -= 4;
            flagGroup = ltlData & 0b10000000 ? true : false;
            flagBSM = ltlData & 0b00000001 ? true : false;
            maxSegLen = fileReadWord();
            len -= 2;
            break;
        case 6:
            printf("SEGDEF describes a load-time locatable (LTL), paragraph aligned LSEG if not member of any group\n");
            ltlData = fileReadByte();
            len--;
            flagGroup = ltlData & 0b10000000 ? true : false;
            flagBSM = ltlData & 0b00000001 ? true : false;
            //maxSegLen = fileReadWord();
            if (flagBSM) {
                maxSegLen = fileReadByte();
                len--;
            } else {
                maxSegLen = fileReadWord();
                len -= 2;
            }
            groupOffset = fileReadWord();
            len -= 2;
            segLen = fileReadWord();
            len -= 2;

            data = fileReadByte();
            len--;
            if (data > 127) {
                segmentNameIndex = (data & 0x7F) << 8;
                data = fileReadByte();
                len--;
                segmentNameIndex += data;
            } else {
                segmentNameIndex = data;
            }

            data = fileReadByte();
            len--;
            if (data > 127) {
                groupNameIndex = (data & 0x7F) << 8;
                data = fileReadByte();
                len--;
                groupNameIndex += data;
            } else {
                groupNameIndex = data;
            }

            data = fileReadByte();
            len--;
            if (data > 127) {
                overlayNameIndex = (data & 0x7F) << 8;
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

    if (aligment == 2) {
        printf("Segment Length:\t %u\t0x%04X\n", segLen, segLen);
        printf("Segment Name Index:\t %u\t0x%04X\n", segmentNameIndex, segmentNameIndex);
        printf("Group Name Index:\t %u\t0x%04X\n", groupNameIndex, groupNameIndex);
        printf("Overlay Name Index:\t %u\t0x%04X\n", overlayNameIndex, overlayNameIndex);

    }
    
    if (aligment == 5 ) {
        printf("LTL Data (RAW): 0x%02X\n", ltlData);
        if (flagGroup) {
            printf("Flag: GROUP. Segment is part of group and should be loaded as part of the group\n");
        }
        if (flagBSM) {
            printf("Flag: BSM. Maximum segment length is 65536. MAXIMUM SEGMENT LENGTH must be 0\n");
        }
        printf("Max Segment Length:\t %u\t0x%04X\n", maxSegLen, maxSegLen);
        if (flagBSM && maxSegLen != 0) {
            printf("Warining: Flag: BSM is set. Max Segment Length MUST BE 0\n");
        }
        
        printf("Frame number:\t%u\t0x%04X\n", frameNumber, frameNumber);
        printf("Offset:\t%u\n", offset);
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
        if (flagBSM && maxSegLen != 0) {
            printf("Warining: Flag: BSM is set. Max Segment Length MUST BE 0\n");
        }
        
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
                printf("Empty leaf:\t%u\n", data);
                break;
            case 128:
                printf("NULL leaf:\t%u\n", data);
                break;
            default:
                printf("Warning: leaf parsing not implemented!. RAW value:\t0x%02X\n", data);
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
        segmentIndex = (data & 0x7F) << 8;
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
    
    char fn[255];
    char buffer[20];
    
    strcpy(fn, sourceFile);
    
    strcat(fn,".recLEDATA.segmentIndex.");
    sprintf(buffer, "%05d", segmentIndex);
    strcat(fn, buffer);
    
    strcat(fn,".offset.");
    sprintf(buffer, "%05d", enumeratedDataOffset);
    strcat(fn, buffer);
    
    strcat(fn,".bin");
    
    printf("Dest File file: %s\n", fn);
    remove(fn);
    printf("Data size:\t%u\t0x%04X\n", len, len);
    printf("Data:\t");
    
    while (len) {
        uint8_t data;
        data = fileReadByte();
        len--;
        printf("%02X ", data);
        fileWriteByte(fn, data);
    }
    printf("\n");
    
    return 0;
}

uint8_t parserGRPDEF(uint16_t len) {
    uint8_t data;
    uint16_t groupNameIndex;
    uint8_t groupComponentDescriptor;
    uint16_t segmentIndex;
    
    uint16_t entryCount = 0;
    
    data = fileReadByte();
    len--;
    if (data > 127) {
        groupNameIndex = (data & 0x7F) << 8;
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
        printf("Entry: %u\n", entryCount+1);
        entryCount++;
        switch (groupComponentDescriptor) {
            case groupSI:
                data = fileReadByte();
                len--;
                if (data > 127) {
                    segmentIndex = (data & 0x7F) << 8;
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
                ltlBGL = (ltlData & 0b00000010) ? true : false;
                ltlBGM = (ltlData & 0b00000001) ? true : false;
                
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
    printf("Total Entries: %u\n", entryCount);
    return 0;
}

uint8_t parserREDATA(uint16_t len) {
    uint8_t data;
    
    uint16_t groupIndex = 0;
    uint16_t segmentIndex = 0;
    uint16_t frameIndex = 0;
    uint16_t dataRecordOffset;
    
    data = fileReadByte();
    len--;
    if (data > 127) {
        groupIndex = (data & 0x7F) << 8;
        data = fileReadByte();
        len--;
        groupIndex += data;
    } else {
        groupIndex = data;
    }
    
    data = fileReadByte();
    len--;
    if (data > 127) {
        segmentIndex = (data & 0x7F) << 8;
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
                frameIndex = (data & 0x7F) << 8;
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

    char fn[255];
    char buffer[20];
    
    strcpy(fn, sourceFile);
    strcat(fn,".recREDATA.groupIndex.");
    sprintf(buffer, "%d", groupIndex);
    strcat(fn, buffer);
    
    strcat(fn,".segmentIndex.");
    sprintf(buffer, "%05d", segmentIndex);
    strcat(fn, buffer);

    strcat(fn,".frameIndex.");
    sprintf(buffer, "%05d", frameIndex);
    strcat(fn, buffer);

    strcat(fn,".offset.");
    sprintf(buffer, "%05d", dataRecordOffset);
    strcat(fn, buffer);
    
    strcat(fn,".bin");
    
    printf("Dest File file: %s\n", fn);
    remove(fn);


    printf("Data size:\t%u\t0x%04X\n", len, len);
    printf("Data:\t");
    while (len) {
        uint8_t data;
        data = fileReadByte();
        len--;
        printf("%02X ", data);
        fileWriteByte(fn, data);
    }
    printf("\n");
    
    return 0;
}

uint8_t parserEXTDEF(uint16_t len) {
    uint16_t i = 0;
    uint8_t data;
    uint8_t strLen = 0;
    uint8_t entryCount = 0;
    uint16_t typeIndex;
    
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
        typeIndex = fileReadByte();
        printf("Type index: 0x%02X\n", typeIndex);
        i++;
        //printf("tmpStr: %s\n", tmpStr);
        
    }
    
    printf("Entry count: %u\n", entryCount);
    return 0;
}

uint8_t parserPUBDEF(uint16_t len) {
    uint8_t data;
    uint8_t strLen;
    //uint8_t printCount = 0;
    
    uint16_t groupIndex;
    //uint8_t groupComponentDescriptor;
    uint16_t segmentIndex;
    uint16_t frameNumber;
    uint16_t offset;
    
    uint16_t entryCount = 0;
    
    data = fileReadByte();
    len--;
    if (data > 127) {
        groupIndex = (data & 0x7F) << 8;
        data = fileReadByte();
        len--;
        groupIndex += data;
    } else {
        groupIndex = data;
    }
    printf("Group Index:\t%u\t0x%04X\n", groupIndex, groupIndex);
    
    data = fileReadByte();
    len--;
    if (data > 127) {
        segmentIndex = (data & 0x7F) << 8;
        data = fileReadByte();
        len--;
        segmentIndex += data;
    } else {
        segmentIndex = data;
    }
    printf("Segment Index:\t0x%04X\n", segmentIndex);
    
    if (groupIndex == 0 && segmentIndex == 0 ) {
        frameNumber = fileReadWord();
        len -=2;
        printf("Frame Number:\t0x%04X\n", frameNumber);
    }
    
    while (len) {
        entryCount++;
        strLen = fileReadByte();
        len--;
        
        printf("\nEntry:\t%u:\t", entryCount);
        
        for (uint8_t i = 0; i < (strLen); i++) {
            data = fileReadByte();
            len--;
            printf("%c", data);
        }
        printf("\n");
        offset = fileReadWord();
        len -= 2;
        printf("Offset:\t%u\n", offset);
        
        data = fileReadByte();
        len--;
        printf("Type:\t");
        switch (data) {
            case 0 ... 127:
                printf("Empty leaf:\t%u\n", data);
                break;
            case 128:
                printf("NULL leaf:\t%u\n", data);
                break;
            default:
                printf("Warning: leaf parsing not implemented!. RAW value:\t0x%02X\n", data);
                break;
        }
    
    }
    printf("\nTotal entries\t%u\n", entryCount);
    
    printf("\n");
    return 0;
}

uint8_t parserFIXUPP(uint16_t len) {
    uint8_t data;
    uint16_t entryCount = 0;

    bool isFixUp = false;
    
    bool d = false;
    uint8_t method;
    uint8_t thred;
    
    uint16_t locat;
    uint8_t fixDat;
    bool m;
    bool s;
    uint8_t loc;
    uint16_t dataRecordOffset;
    
    bool f;
    uint8_t frame;
    bool t;
    bool p;
    bool targt;
    
    uint16_t frameDatum;
    uint16_t targetDatum;
    uint32_t targetDisplacement;

    while(len) {
        entryCount++;
        data = fileReadByte();
        len--;
        
        isFixUp = (data & 0b10000000) ? true : false;
        
        printf("Entry:\t%u\n", entryCount);
        
        printf("Type:\t");
        if (isFixUp) { // Fixup field
            printf("Fixup\n");
            
            locat = data << 8;
            locat = locat | fileReadByte();
            len--;
            fixDat = fileReadByte();
            len--;
            printf("RAW locat:\t0x%04X\n", locat);
            m = (locat & 0b0100000000000000) ? true : false;
            s = (locat & 0b0010000000000000) ? true : false;
            loc = (locat & 0b0001110000000000) >> 10;
            dataRecordOffset = (locat & 0b0000001111111111);
            if (m) {
                printf("Flag: m: is set.\tSegment relative mode\n");
            } else {
                printf("Flag: m: is clear.\tSelf-relative mode\n");
            }
            if (s) {
                printf("Flag: s: is set.\t24-bit singed\n");
            } else {
                printf("Flag: s: is clear.\t16-bit unsinged\n");
            }
            printf("Fix type (loc):\t%u\nFix type (loc):\t", loc);
            switch (loc) {
                case locLoByte:
                    printf("Low Byte\n");
                    break;
                case locOffset:
                    printf("Offset\n");
                    break;
                case locBase:
                    printf("Base\n");
                    break;
                case locPointer:
                    printf("Pointer\n");
                    break;
                case locHiByte:
                    printf("High Byte\n");
                    break;
                default:
                    printf("Error\n");
                    break;
            }
            printf("Data Record Offset:\t%u\n", dataRecordOffset);
            
            printf("RAW fixDat:\t0x%02X\n", fixDat);        
            f = (fixDat & 0b10000000) ? true : false;
            frame = (fixDat & 0b01110000) >> 4;
            t = (fixDat & 0b00001000) ? true : false;
            p = (fixDat & 0b00000100) ? true : false;
            targt = (fixDat & 0b00000011);
            
            
            if (f) {
                printf("Flag: f: is set.\tFrame specified explicitly\n");
            } else {
                printf("Flag: f: is clear.\tFrame specified by thread\n");
            }
            printf("Frame:\t%u\n", frame);
            if (t) {
                printf("Flag: t: is set.\tTarget specified by reference to thread\n");
            } else {
                printf("Flag: t: is clear.\tTarget specified explicitly\n");
            }
            if (p) {
                printf("Flag: p: is set.\tTarget is specified in primary way\n");
            } else {
                printf("Flag: p: is clear.\tTarget is specified in secondary way\n");
            }
            printf("Target:\t%u\n", targt);
            
            if (t) {
                thred = targt;
                printf("Thread %u\n", thred);
            } else {
                method = targt;
                if (p) {
                    method = targt | 0b100;
                }
                printf("Method:\t(T%01X)\n", method);
            }
            
            if (f) {
                data = fileReadByte();
                len--;
                if (data > 127) {
                    frameDatum = data << 8;
                    data = fileReadByte();
                    len--;
                    frameDatum += data;
                } else {
                    frameDatum = data;
                }
                printf("Frame Datum:\t%u\t0x%04X\n", frameDatum, frameDatum);            
            }
            
            if (!t) {
                data = fileReadByte();
                len--;
                if (data > 127) {
                    targetDatum = data << 8;
                    data = fileReadByte();
                    len--;
                    targetDatum += data;
                } else {
                    targetDatum = data;
                }
                printf("Target Datum:\t%u\t0x%04X\n", targetDatum, targetDatum);            
            }
            
            if (!p) {
                data = fileReadByte();
                len--;
                if (data > 127) {
                    targetDisplacement = data << 16;
                    data = fileReadByte();
                    len--;
                    targetDisplacement += data << 8;
                    len--;
                    targetDisplacement += data;
                } else {
                    targetDisplacement = data;
                }
                printf("Target Displacement:\t%u\t0x%04X\n", targetDisplacement, targetDisplacement);            
            }
            
            
        } else {
            // Thread field
            printf("Thread\n");
            printf("RAW thread header:\t0x%02X\n", data);
            d = data & 0b01000000 ? true : false;
            method = (data & 0b00011100) >> 2;
            thred = data & 0b00000011;
            
            if (d) {
                printf("Flag: D: is reset\n");
                printf("Flag: D: mod4 method (T%01X)\tNeed to fix mod4 stuff\n", method);
                printf("RAW method:\t%u\n", method);

            } else {
                printf("Flag: D: is set\n");
                printf("Flag: D: normal method (F%01X)\n", method);
            }
            printf("Thred:\t%u\n", thred);
            printf("Warning: not implemented. Need more codding.\n");
        }
    printf("\n");
    }

    printf("Total entries: %u\n", entryCount);
    
    if (len) printf("Warning: parse errors!\nDumping remaining data:\n");
    while (len) {
        data = fileReadByte();
        len--;
        printf("%02X ", data);
    }
    printf("\n");
    return 0;
}

uint8_t parserBLKEND(uint16_t len) {
    printf("Lol! Nothing here!\n");
    return 0;
}

uint8_t parserLIBHED(uint16_t len) {
    uint8_t data;
    
    uint16_t moduleCount;
    uint16_t blockNumber;
    uint16_t byteNumber;
    
    moduleCount = fileReadWord();
    len -= 2;
    blockNumber = fileReadWord();
    len -= 2;
    byteNumber = fileReadWord();
    len -= 2;
    
    printf("Module count:\t%u\n", moduleCount);
    printf("Block Number:\t%u\n", blockNumber);
    printf("Byte Number:\t%u\n", byteNumber);
    
    if (len) {
        printf("Warning! Record too long!\nDumping tail:\n");
    }
    
    while (len) {
        data = fileReadByte();
        len--;
        printf("%02X ", data);
    }
    printf("\n");
    return 0;
}

uint8_t parserLIBNAM(uint16_t len) {
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

uint8_t parserLIBLOC(uint16_t len) {
    uint16_t blockNumber;
    uint16_t byteNumber;
    uint16_t entryCount = 0;
    while (len) {
        blockNumber = fileReadWord();
        byteNumber = fileReadWord();
        len -= 4;
        entryCount ++;
        printf("Entry: %u:\t", entryCount);
        printf("Block Number: %u\t0x%04X\t\t", blockNumber, blockNumber);
        printf("Byte Number: %u\t0x%04X\n", byteNumber, byteNumber);
    }
    printf("Entry count: %u\n", entryCount);
    return 0;
}

uint8_t parserLIBDIC(uint16_t len) {
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
