#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "fileOps.h"
#include "calcXsum.h"

FILE *ptrFile;

FILE *fileOpen(char *fileName){
    
    //FILE *ptr_myfile;
    ptrFile = fopen(fileName,"rb");
    if (!ptrFile)
    {
        printf("Unable to open file!\n");
        exit(-1);
    }
    return ptrFile;
}

uint8_t fileReadByte() {
    uint8_t retVal;
    fread(&retVal, sizeof(retVal), 1, ptrFile);
    calcXsumFeed(retVal);
    return retVal;
}

uint16_t fileReadWord() {
    uint16_t retVal;
    fread(&retVal, sizeof(retVal), 1, ptrFile);
    calcXsumFeed( (retVal & 0xFF00) >> 8 );
    calcXsumFeed( retVal & 0xFF);
    return retVal;
}

uint32_t fileReadLong() {
    uint32_t retVal;
    fread(&retVal, sizeof(retVal), 1, ptrFile);
    calcXsumFeed( (retVal & 0xFF000000) >> 24 );
    calcXsumFeed( (retVal & 0xFF0000) >> 16 );
    calcXsumFeed( (retVal & 0xFF00) >> 8 );
    calcXsumFeed( retVal & 0xFF);
    return retVal;
}

long int fileGetCurrentOffset() {
    return ftell(ptrFile);
}

bool fileIsEOF() {
    return feof(ptrFile);
}

void fileClose() {
    fclose(ptrFile);
}
