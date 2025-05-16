#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "fileOps.h"
#include "calcXsum.h"

FILE *ptrFile;

char sourceFile[255];

FILE *fileOpen(char *fileName){
    
    //FILE *ptr_myfile;
    ptrFile = fopen(fileName,"rb");
    if (!ptrFile)
    {
        printf("Unable to open file!\n");
        exit(-1);
    }
    strcpy(sourceFile, fileName);
    return ptrFile;
}

uint8_t fileReadByte() {
    uint8_t retVal;
    size_t err;
    
    err = fread(&retVal, sizeof(retVal), 1, ptrFile);
    if (err != 1) {
        printf("\n");
        exit(-1);
    }
    calcXsumFeed(retVal);
    return retVal;
}

uint16_t fileReadWord() {
    uint16_t retVal;
    size_t err;
    
    err = fread(&retVal, sizeof(retVal), 1, ptrFile);
    if (err != 1) {
        printf("\n");
        exit(-1);
    }
    calcXsumFeed( (retVal & 0xFF00) >> 8 );
    calcXsumFeed( retVal & 0xFF);
    return retVal;
}

uint32_t fileReadLong() {
    uint32_t retVal;
    size_t err;
    
    err = fread(&retVal, sizeof(retVal), 1, ptrFile);
    if (err != 1) {
        printf("\n");
        exit(-1);
    }
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

bool fileWriteByte(const char *fileName, uint8_t data) {
    FILE *ptrFileWrite;
    ptrFileWrite = fopen(fileName,"a");
    if (!ptrFile)
    {
        printf("Unable to open file: %s\n", fileName);
        return false;
    }
    fwrite( &data, 1, 1, ptrFileWrite);
    fclose(ptrFileWrite);
    return true;
}
