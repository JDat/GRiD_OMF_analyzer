#ifndef _fileOps_h
#define _fileOps_h

    FILE *fileOpen(char *fileName);
    uint8_t fileReadByte();
    uint16_t fileReadWord();
    uint32_t fileReadLong();
    long int fileGetCurrentOffset();
    bool fileIsEOF();
    void fileClose();

#endif
