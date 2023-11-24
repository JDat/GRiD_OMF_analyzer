#ifndef _rectypes_h
#define _rectypes_h

// (empty) - not implemented yet for some reason
//      *  - implemented, kinda working, small chance that it have bugs, need double checking
//     ** - partially implemented, need more checking/coding
// numerical order from OMF document. See page 88 (PDF page 90).
    #define RHEADR  0x6e        // * used
    #define REGINT  0x70        // ** used, Partial implementation, need more coding
    #define REDATA  0x72        // * used
    #define RIDATA  0x74        // used
    #define OVLDEF  0x76        // used
    #define ENDREC  0x78        // * used

    #define BLKDEF  0x7a
    #define BLKEND  0x7c        // * not used. Easy to implement
    #define DEBSYM  0x7e

    #define THEADR  0x80        // ** used, need some fixing
    #define LHEADR  0x82        // ** used, need some fixing
    #define PEDATA  0x84        // * used
    #define PIDATA  0x86        // used
    #define COMENT  0x88        // ** not used, present in some files, need some fixing
    #define MODEND  0x8a        // ** used, Partial implementation, need more coding

    #define EXTDEF  0x8c        // * not used, present in some files
    #define TYPDEF  0x8e        // * not used, present in font file
    #define PUBDEF  0x90        // ** not used, present in some files, Partial implementation, need more coding
    #define LOCSYM  0x92        //
    #define LINNUM  0x94        //
    #define LNAMES  0x96        // ** not used, need some fixing

    #define SEGDEF  0x98        // ** used, Partial implementation
    #define GRPDEF  0x9a        // ** used, Partial implementation, need more coding
    #define FIXUPP  0x9c        // ** used, Partial implementation, need more coding

    #define R_NONE  0x9e        // no info about this record
    #define LEDATA  0xa0        // * not used in executble. Used in font file for data
    #define LIDATA  0xa2        // not used
    #define LIBHED  0xa4        // * not used
    #define LIBNAM  0xa6        // * not used
    #define LIBLOC  0xa8        // * not used, have in libraries
    #define LIBDIC  0xaa        // * not used, have in libraries

    #define parseErrType    0x00    // for unknow unparsable error. Fall back to dump record data
    
    uint8_t searchForParser(uint8_t val);    
    uint8_t parserErr(uint16_t len);
    
    // In order of implementation
    uint8_t parserRHEADR(uint16_t len);
    uint8_t parserREGINT(uint16_t len);
    uint8_t parserMODEND(uint16_t len);
    uint8_t parserTHEADR(uint16_t len);
    uint8_t parserLHEADR(uint16_t len);
    uint8_t parserCOMENT(uint16_t len);
    uint8_t parserSEGDEF(uint16_t len);
    uint8_t parserLNAMES(uint16_t len);
    uint8_t parserTYPDEF(uint16_t len);
    uint8_t parserLEDATA(uint16_t len);
    uint8_t parserGRPDEF(uint16_t len);
    uint8_t parserREDATA(uint16_t len);
    uint8_t parserEXTDEF(uint16_t len);
    uint8_t parserPUBDEF(uint16_t len);
    uint8_t parserFIXUPP(uint16_t len);
    uint8_t parserBLKEND(uint16_t len);
    uint8_t parserLIBHED(uint16_t len);
    uint8_t parserLIBNAM(uint16_t len);
    uint8_t parserLIBLOC(uint16_t len);
    uint8_t parserLIBDIC(uint16_t len);
    uint8_t parserENDREC(uint16_t len);
    uint8_t parserPEDATA(uint16_t len);
    
    
    typedef uint8_t (*funcPtr)(uint16_t len);
    
    struct recordType {
        uint8_t recType;
        funcPtr parseFn;
        const char *recordName;
    };

    const struct recordType recordTypes[] = {
        {parseErrType,  parserErr,       "Record type not supported"},
        {RHEADR,        parserRHEADR,    "R-module Header Record"},
        {REGINT,        parserREGINT,    "Register Initialisation Record"},
        {MODEND,        parserMODEND,    "Module end Record"},
        {THEADR,        parserTHEADR,    "T-module Header Record"},
        {LHEADR,        parserLHEADR,    "L-module Header Record"},
        {COMENT,        parserCOMENT,    "Comment Record"},
        {SEGDEF,        parserSEGDEF,    "Segment Definition Record"},
        {LNAMES,        parserLNAMES,    "List of Names Record"},
        {TYPDEF,        parserTYPDEF,    "Type Definition Record"},
        {LEDATA,        parserLEDATA,    "Logical Enumerated Data Record"},
        {GRPDEF,        parserGRPDEF,    "Group Definition Record"},
        {REDATA,        parserREDATA,    "Relocatable Enumerated Data Record"},
        {EXTDEF,        parserEXTDEF,    "External Names Definition Record"},
        {PUBDEF,        parserPUBDEF,    "Public Names Definition Record"},
        {FIXUPP,        parserFIXUPP,    "Fixup Record"},
        {BLKEND,        parserBLKEND,    "Block End Record"},
        {LIBHED,        parserLIBHED,    "Library Header Record"},
        {LIBNAM,        parserLIBNAM,    "Library Modile Names Record"},
        {LIBLOC,        parserLIBLOC,    "Library Modile Locations Record"},
        {LIBDIC,        parserLIBDIC,    "Library Dictionary Record"},
        {ENDREC,        parserENDREC,    "End Record"},
        {PEDATA,        parserPEDATA,    "End Record"}
    };
    
    // Group component descriptors
    #define groupSI     0xff
    #define groupEI     0xfe
    #define groupSCO    0xfd
    #define groupLTL    0xfb
    #define groupABS    0xfa
    
    // loc component (in Fixup record) descriptors
    #define locLoByte   0
    #define locOffset   1
    #define locBase     2
    #define locPointer  3
    #define locHiByte   4
    
#endif

// order from OMF document by pages
/*
    #define THEADR  0x80
    #define LHEADR  0x82
    #define RHEADR  0x6e
    #define LNAMES  0x96
    #define SEGDEF  0x98
    #define GRPDEF  0x9a
    #define TYPDEF  0x8e
    #define PUBDEF  0x90
    #define EXTDEF  0x8c
    #define LOCSYM  0x92
    #define LINNUM  0x94
    #define BLKDEF  0x7a
    #define BLKEND  0x7c
    #define DEBSYM  0x7e
    #define REDATA  0x72
    #define RIDATA  0x74
    #define PEDATA  0x84
    #define PIDATA  0x86
    #define LEDATA  0xa0
    #define LIDATA  0xa2
    #define FIXUPP  0x9c
    #define OVLDEF  0x76
    #define ENDREC  0x78
    #define REGINT  0x70
    #define MODEND  0x8a
    #define LIBHED  0xa4
    #define LIBNAM  0xa6
    #define LIBLOC  0xa8
    #define LIBDIC  0xaa
    #define COMENT  0x88

    #define R_NONE  0x9e

From LDRLTL.PLM
    CALL ProcessREGINT;      // REGINT
    CALL ProcessREDATA;      // REDATA
    CALL ProcessRIDATA;      // RIDATA
    CALL IgnoreRecord;       // OVLDEF
    CALL ProcessENDREC;      // ENDREC
    CALL IgnoreRecord;       // BLKDEF
    CALL IgnoreRecord;       // BLKEND
    CALL IgnoreRecord;       // DEBSYM
    CALL IgnoreRecord;       // THEADR
    CALL LoaderException (eInvRec);    // LHEADR
    CALL ProcessPEDATA;      // PEDATA
    CALL ProcessPIDATA;      // PIDATA
    CALL IgnoreRecord;       // COMENT
    CALL ProcessMODEND;      // MODEND
    CALL ProcessEXTDEF;      // EXTDEF
    CALL IgnoreRecord;       // TYPDEF
    CALL ProcessPUBDEF;      // PUBDEF
    CALL IgnoreRecord;       // LOCSYM
    CALL IgnoreRecord;       // LINNUM
    CALL IgnoreRecord;       // LNAMES
    CALL ProcessSEGDEF;      // SEGDEF
    CALL ProcessGRPDEF;      // GRPDEF
    CALL pProcessFIXUPP (loadMemoryPtr);  // FIXUPP

From file LDROVL.PLM
  The main loop handles only **DATA and FIXUPP   
  records.  ENDREC (overlay) terminates the loop.
  All other records are ignored.                 

From file ldr.inc
            OMF Record Format Codes             

DCL rHeadr LIT '6EH';
DCL regInt LIT '70H';
DCL reData LIT '72H';
DCL riData LIT '74H';
DCL ovlDef LIT '76H';
DCL endRec LIT '78H';
DCL tHeadr LIT '80H';
DCL lHeadr LIT '82H';
DCL peData LIT '84H';
DCL piData LIT '86H';
DCL modEnd LIT '8AH';
DCL fixUpp LIT '9CH';
  

From file romtypes.inc
        OMF Record types

  DCL rHeadr             LIT '6EH';
  DCL regInt             LIT '70H';
  DCL reData             LIT '72H';
  DCL riData             LIT '74H';
  DCL ovlDef             LIT '76H';
  DCL endRec             LIT '78H';
  DCL tHeadr             LIT '80H';
  DCL lHeadr             LIT '82H';
  DCL peData             LIT '84H';
  DCL piData             LIT '86H';
  DCL modEnd             LIT '8AH';
  DCL fixUpp             LIT '9CH';

  DCL found              LIT '0ffffh';
  DCL notFound           LIT '0';
  
*/
