#ifndef _rectypes_h
#define _rectypes_h
// numerical order
    #define RHEADR  0x6e        //used *
    #define REGINT  0x70        //used *
    #define REDATA  0x72        //used
    #define RIDATA  0x74        //used
    #define OVLDEF  0x76        //used
    #define ENDREC  0x78        //used

    #define BLKDEF  0x7a
    #define BLKEND  0x7c
    #define DEBSYM  0x7e

    #define THEADR  0x80        //used ** Partial implementation
    #define LHEADR  0x82        //used
    #define PEDATA  0x84        //used
    #define PIDATA  0x86        //used
    #define COMENT  0x88        //have, but not used *
    #define MODEND  0x8a        //used *

    #define EXTDEF  0x8c
    #define TYPDEF  0x8e
    #define PUBDEF  0x90
    #define LOCSYM  0x92
    #define LINNUM  0x94
    #define LNAMES  0x96

    #define SEGDEF  0x98        //used
    #define GRPDEF  0x9a        //used
    #define FIXUPP  0x9c        //used

    #define R_NONE  0x9e
    #define LEDATA  0xa0
    #define LIDATA  0xa2
    #define LIBHED  0xa4
    #define LIBNAM  0xa6
    #define LIBLOC  0xa8
    #define LIBDIC  0xaa

// order from OMF document
/*
    #define THEADR  0x80        //used ** Partial implementation
    #define LHEADR  0x82        //used
    #define RHEADR  0x6e        //used *
    #define LNAMES  0x96
    #define SEGDEF  0x98        //used
    #define GRPDEF  0x9a        //used
    #define TYPDEF  0x8e
    #define PUBDEF  0x90
    #define EXTDEF  0x8c
    #define LOCSYM  0x92
    #define LINNUM  0x94
    #define BLKDEF  0x7a
    #define BLKEND  0x7c
    #define DEBSYM  0x7e
    #define REDATA  0x72        //used
    #define RIDATA  0x74        //used
    #define PEDATA  0x84        //used
    #define PIDATA  0x86        //used
    #define LEDATA  0xa0
    #define LIDATA  0xa2
    #define FIXUPP  0x9c        //used
    #define OVLDEF  0x76        //used
    #define ENDREC  0x78        //used
    #define REGINT  0x70        //used *
    #define MODEND  0x8a        //used *
    #define LIBHED  0xa4
    #define LIBNAM  0xa6
    #define LIBLOC  0xa8
    #define LIBDIC  0xaa
    #define COMENT  0x88        //have, but not used *

    #define R_NONE  0x9e
*/

    #define parseErrType    0x00    // for unknow unparsable error. Fall back to dump record data
    typedef uint8_t (*funcPtr)(uint16_t len);

    uint8_t searchForParser(uint8_t val);
    
    uint8_t parserErr(uint16_t len);
    
    uint8_t parserRHEADR(uint16_t len);
    uint8_t parserREGINT(uint16_t len);
    uint8_t parserMODEND(uint16_t len);
    uint8_t parserTHEADR(uint16_t len);
    uint8_t parserCOMENT(uint16_t len);
    uint8_t parserSEGDEF(uint16_t len);
    //uint8_t parserLEDATA (uint16_t len);
    
    struct recordType {
        uint8_t recType;
        funcPtr parseFn;
        const char *recordName;
    };

    const struct recordType recordTypes[] = {
        {parseErrType,  parserErr,       "Record type not supported"},
        {RHEADR,        parserRHEADR,    "R-module Header Record"},
        {REGINT,        parserREGINT,    "Register initialisation Record"},
        {MODEND,        parserMODEND,    "Module end Record"},
        {THEADR,        parserTHEADR,    "Т-module Header Record"},
        {COMENT,        parserCOMENT,    "Comment Record"},
        {SEGDEF,        parserSEGDEF,    "Segment definition Record"}
        //{LEDATA,        parserLEDATA,    "Logical enumerated data record"},
    };
#endif
