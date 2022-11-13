#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "recTypes.h"
#include "fileOps.h"
#include "calcXsum.h"

int main(int argc, char *argv[])
{

    if (argc != 2 or strlen(argv[1]) == 0 ) {
        printf("Provide filename for analyze:\n");
        printf("./analyze FILENAME.RUN\n");
        exit(-1);
    }

    fileOpen(argv[1]);
    printf("Filename:\t%s\n\n", argv[1]);

    while (1) {
        uint8_t recType;    
        uint16_t recSize;
        uint8_t xSumRead;
        uint8_t xSum;
        uint32_t offset = 0;

        calcXsumInit();
        offset  = fileGetCurrentOffset();
        recType = fileReadByte();
        if ( fileIsEOF() ) {
            //printf("End of file. Maybe something broken in OMF records\n");
            return 0;
        }
        printf("Offset:\t0x%05X\n", offset);
        printf("Type:\t0x%02X\n", recType);


        recSize = fileReadWord();
        printf("Size:\t%u\t0x%02X\n", recSize, recSize);

        uint8_t j = searchForParser(recType);
        recordTypes[j].parseFn(recSize - 1);

        xSum = calcXsumResult();
        xSumRead = fileReadByte();
        printf("xSum:\t0x%02X", xSumRead);
        if (xSumRead == xSum) {
            printf(" (OK)\n\n");
        } else {
            printf(" (Wrong)! Calculated: 0x%02X\n\n", xSum);
        }
    }

    fileClose();
    return 0;
}
