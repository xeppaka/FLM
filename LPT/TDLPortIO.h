//*** TDLPortIO: DriverLINX Port IO Driver wrapper DLL ***********************
//**                                                                        **
//** File: TDLPortIO.h                                                      **
//**                                                                        **
//** Copyright (c) 1999 John Pappas (DiskDude). All rights reserved.        **
//**     This software is FreeWare.                                         **
//**                                                                        **
//**     Please notify me if you make any changes to this file.             **
//**     Email: diskdude@poboxes.com                                        **
//**                                                                        **
//*** http://diskdude.cjb.net/ ***********************************************

#ifndef __TDLPortIO_H__
#define __TDLPortIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef TDLPortIO_LIBRARY
/* DLL export */
#define EXPORT __declspec(dllexport)
#else
/* EXE import */
#define EXPORT __declspec(dllimport)
#endif



//---------------------------------------------------------------------------
// Constants
//---------------------------------------------------------------------------

// Specifies the type of read or write in a TPortCommand
#define tmReadByte    1
#define tmReadWord    2
#define tmReadDWord   3
#define tmWriteByte   4
#define tmWriteWord   5
#define tmWriteDWord  6

// Maximum number of printer ports that would be installed on a system
#define MAX_LPT_PORTS 8


//---------------------------------------------------------------------------
// Data Types
//---------------------------------------------------------------------------


// Specifies the data required to do a block
// read/write of an array of port records.
// Extends the model TVicHW32/TVicPort uses
typedef struct
{
   WORD PortAddr;    // The address of the port to read/write
   union             // The data to read/write
   {
      BYTE  Byte;
      WORD  Word;
      DWORD DWord;
   } PortData;
   BYTE PortMode;    // The mode of reading/writing
} TPortCommand;

// Standard TVicHW32/TVicPort PortRec for compatibility
typedef struct
{
   WORD PortAddr;
   BYTE PortData;
   bool fWrite;
} TPortRec;


//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

// These open and close the DLL/Driver
 EXPORT void __stdcall OpenLPTDriver(void);
 EXPORT void __stdcall CloseLPTDriver(void);

// Allows write/read array of ports.
 EXPORT void __stdcall PortControl(TPortRec*, WORD);
 EXPORT void __stdcall PortCommand(TPortCommand*, WORD);

// Allows read/write array of bytes from single port.
 EXPORT void __stdcall ReadPortFIFO(WORD,  WORD, BYTE*);
 EXPORT void __stdcall WritePortFIFO(WORD, WORD, BYTE*);

// Extended block read/write routines for WORD and DWORD
 EXPORT void __stdcall ReadWPortFIFO(WORD,  WORD, WORD*);
 EXPORT void __stdcall WriteWPortFIFO(WORD, WORD, WORD*);
 EXPORT void __stdcall ReadLPortFIFO(WORD,  WORD, DWORD*);
 EXPORT void __stdcall WriteLPortFIFO(WORD, WORD, DWORD*);

// Sets the path (no ending \, nor any filename) of the DLPortIO.SYS file
// Assumed to be <windows system directory>\DRIVERS if not specified
 EXPORT char* __stdcall GetDriverPath(void);
 EXPORT void  __stdcall SetDriverPath(char *Path);

// Sets the path (no ending \, nor any filename) of the DLPortIO.DLL file
// Assumed to be "" if not specified, meaning it will search the program
// path, windows directory and computer's path for the DLL
 EXPORT char* __stdcall GetDLLPath(void);
 EXPORT void  __stdcall SetDLLPath(char *Path);

// True when the DLL/Driver has been loaded successfully after OpenDriver()
 EXPORT bool __stdcall ActiveHW(void);
 EXPORT bool __stdcall IsDriverOpened(void);

// This doesn't really do anything; provided for compatibility only
 EXPORT bool __stdcall TestHardAccess(void);
 EXPORT void __stdcall SetHardAccess(bool);

// Returns the last error which occurred in Open/CloseDriver()
EXPORT char* __stdcall LastError(void);

// Reads and writes ports
EXPORT BYTE  __stdcall ReadPort(WORD);
EXPORT WORD  __stdcall ReadPortW(WORD);
EXPORT DWORD __stdcall ReadPortL(WORD);
EXPORT void  __stdcall WritePort(WORD, BYTE);
EXPORT void  __stdcall WritePortW(WORD, WORD);
EXPORT void  __stdcall WritePortL(WORD, DWORD);


//**-----------------------------------------------------------------------**
// Extended functions for dealing with the printer port
//**-----------------------------------------------------------------------**

// Sends STROBE signal to the printer
void __stdcall LPTStrobe(void);
// Sends AUTOFD (auto line feed) signal to the printer
void __stdcall LPTAutofd(bool);
// Resets printer by sending INIT signal
void __stdcall LPTInit(void);
// Sends SLCTIN signal to the printer
void __stdcall LPTSlctIn(void);
// Sends a character to the printer.
// Returns true on success. Repeat as neccessary.
bool __stdcall LPTPrintChar(char);

// Shows how many LPT ports are installed on your PC.
BYTE __stdcall LPTNumPorts(void);
// Selects the LPT port to use for all LPT operations
BYTE __stdcall GetLPTNumber(void);
void __stdcall SetLPTNumber(BYTE);
// Returns a base address of the current LPT port.
WORD __stdcall LPTBasePort(void);

// Index valid is in the range 1-25 only (other values return false)
// Reading the pin returns true when it is 5V, or false when it at 0V.
// Writing true sets the pin to 5V, or 0V when false.
bool __stdcall GetPin(BYTE);
void __stdcall SetPin(BYTE, bool);

// Returns ACKWL state from the printer
bool __stdcall LPTAckwl(void);
// Returns BUSY state from the printer
bool __stdcall LPTBusy(void);
// Returns PAPER END state from the printer
bool __stdcall LPTPaperEnd(void);
// Returns SLCT state from the printer
bool __stdcall LPTSlct(void);
// Returns ERROR state from the printer
bool __stdcall LPTError(void);

#endif

#ifdef __cplusplus
}
#endif
