#ifndef _fileOps_h
#define _fileOps_h

    extern char sourceFile[255];
    FILE *fileOpen(char *fileName);
    uint8_t fileReadByte();
    uint16_t fileReadWord();
    uint32_t fileReadLong();
    long int fileGetCurrentOffset();
    bool fileIsEOF();
    void fileClose();
    
    bool fileWriteByte(const char *fileName, uint8_t data);

#endif
