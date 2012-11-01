/*

	bladedll.h

    +++++++++++++++++++++++++++
	+   Blade's Encoder DLL   +
	+++++++++++++++++++++++++++

    ------------------------------------------------------
	- Version 1.00 (7 November 1998) - Jukka Poikolainen -
	------------------------------------------------------

	Initial version
	
	------------------------------------------------------
	- Version x.xx (x xxxxxxxx xxxx) - xxxxx xxxxxxxxxxx -
	------------------------------------------------------

*/

#ifndef ___BLADEDLL_H_INCLUDED___
#define ___BLADEDLL_H_INCLUDED___

#pragma pack(push)
#pragma pack(1)

/* encoding formats */

#define		BE_CONFIG_MP3			0										
#define		BE_CONFIG_LAME			256		

/* type definitions */

typedef		unsigned long			HBE_STREAM;
typedef		HBE_STREAM				*PHBE_STREAM;
typedef		unsigned long			BE_ERR;

/* error codes */

#define		BE_ERR_SUCCESSFUL					0x00000000
#define		BE_ERR_INVALID_FORMAT				0x00000001
#define		BE_ERR_INVALID_FORMAT_PARAMETERS	0x00000002
#define		BE_ERR_NO_MORE_HANDLES				0x00000003
#define		BE_ERR_INVALID_HANDLE				0x00000004
#define		BE_ERR_BUFFER_TOO_SMALL				0x00000005

/* other constants */

#define		BE_MAX_HOMEPAGE			256

/* format specific variables */

#define		BE_MP3_MODE_STEREO		0
#define		BE_MP3_MODE_JSTEREO		1
#define		BE_MP3_MODE_DUALCHANNEL	2
#define		BE_MP3_MODE_MONO		3

#define		MPEG1	1
#define		MPEG2	0

#ifdef _BLADEDLL
#undef FLOAT
	#include <Windows.h>
#endif


typedef enum 
{
	NORMAL_QUALITY=0,
	LOW_QUALITY,
	HIGH_QUALITY,
	VOICE_QUALITY
} MPEG_QUALITY;

typedef struct	{
	DWORD	dwConfig;			// BE_CONFIG_XXXXX
								// Currently only BE_CONFIG_MP3 is supported
	union	{

		struct	{

			DWORD	dwSampleRate;	// 48000, 44100 and 32000 allowed
			BYTE	byMode;			// BE_MP3_MODE_STEREO, BE_MP3_MODE_DUALCHANNEL, BE_MP3_MODE_MONO
			WORD	wBitrate;		// 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256 and 320 allowed
			BOOL	bPrivate;		
			BOOL	bCRC;
			BOOL	bCopyright;
			BOOL	bOriginal;

			} mp3;					// BE_CONFIG_MP3

			struct
			{
			// STRUCTURE INFORMATION
			DWORD			dwStructVersion;	
			DWORD			dwStructSize;

			// BASIC ENCODER SETTINGS
			DWORD			dwSampleRate;	// ALLOWED SAMPLERATE VALUES DEPENDS ON dwMPEGVersion 
			DWORD			dwReSampleRate;	// DOWNSAMPLERATE, 0=ENCODER DECIDES  
			INT				nMode;			// BE_MP3_MODE_STEREO, BE_MP3_MODE_DUALCHANNEL, BE_MP3_MODE_MONO
			DWORD			dwBitrate;		// CBR bitrate, VBR min bitrate
			DWORD			dwMaxBitrate;	// CBR ignored, VBR Max bitrate
			MPEG_QUALITY	nQuality;		// Quality setting (NORMAL,HIGH,LOW,VOICE)
			DWORD			dwMpegVersion;	// MPEG-1 OR MPEG-2
			DWORD			dwPsyModel;		// FUTURE USE, SET TO 0
			DWORD			dwEmphasis;		// FUTURE USE, SET TO 0

			// BIT STREAM SETTINGS
			BOOL			bPrivate;		// Set Private Bit (TRUE/FALSE)
			BOOL			bCRC;			// Insert CRC (TRUE/FALSE)
			BOOL			bCopyright;		// Set Copyright Bit (TRUE/FALSE)
			BOOL			bOriginal;		// Set Original Bit (TRUE/FALSE)
			
			// VBR STUFF
			BOOL			bWriteVBRHeader;	// WRITE XING VBR HEADER (TRUE/FALSE)
			BOOL			bEnableVBR;			// USE VBR ENCODING (TRUE/FALSE)
			INT				nVBRQuality;		// VBR QUALITY 0..9

			BYTE			btReserved[255];	// FUTURE USE, SET TO 0

			} LHV1;					// LAME header version 1

		struct	{

			DWORD	dwSampleRate;
			BYTE	byMode;
			WORD	wBitrate;
			BYTE	byEncodingMethod;

		} aac;

	} format;
		
} BE_CONFIG, *PBE_CONFIG;


typedef struct	{

	// BladeEnc DLL Version number

	BYTE	byDLLMajorVersion;
	BYTE	byDLLMinorVersion;

	// BladeEnc Engine Version Number

	BYTE	byMajorVersion;
	BYTE	byMinorVersion;

	// DLL Release date

	BYTE	byDay;
	BYTE	byMonth;
	WORD	wYear;

	// BladeEnc	Homepage URL

	CHAR	zHomepage[BE_MAX_HOMEPAGE + 1];	

} BE_VERSION, *PBE_VERSION;			

#ifndef _BLADEDLL

typedef BE_ERR	(*BEINITSTREAM)		(PBE_CONFIG, PDWORD, PDWORD, PHBE_STREAM);
typedef BE_ERR	(*BEENCODECHUNK)	(HBE_STREAM, DWORD, PSHORT, PBYTE, PDWORD);
typedef BE_ERR	(*BEDEINITSTREAM)	(HBE_STREAM, PBYTE, PDWORD);
typedef BE_ERR	(*BECLOSESTREAM)	(HBE_STREAM);
typedef VOID	(*BEVERSION)		(PBE_VERSION);

#define	TEXT_BEINITSTREAM	"beInitStream"
#define	TEXT_BEENCODECHUNK	"beEncodeChunk"
#define	TEXT_BEDEINITSTREAM	"beDeinitStream"
#define	TEXT_BECLOSESTREAM	"beCloseStream"
#define	TEXT_BEVERSION		"beVersion"

/*	
	BE_ERR	beInitStream(PBE_CONFIG pbeConfig, PDWORD dwSamples, PDWORD dwBufferSize, PHBE_STREAM phbeStream);
	BE_ERR	beEncodeChunk(HBE_STREAM hbeStream, DWORD nSamples, PSHORT pSamples, PBYTE pOutput, PDWORD pdwOutput);
	BE_ERR	beDeinitStream(HBE_STREAM hbeStream, PBYTE pOutput, PDWORD pdwOutput);
	BE_ERR	beCloseStream(HBE_STREAM hbeStream);
	VOID	beVersion(PBE_VERSION pbeVersion);		
*/
	
#else

__declspec(dllexport) BE_ERR	beInitStream(PBE_CONFIG pbeConfig, PDWORD dwSamples, PDWORD dwBufferSize, PHBE_STREAM phbeStream);
__declspec(dllexport) BE_ERR	beEncodeChunk(HBE_STREAM hbeStream, DWORD nSamples, PSHORT pSamples, PBYTE pOutput, PDWORD pdwOutput);
__declspec(dllexport) BE_ERR	beDeinitStream(HBE_STREAM hbeStream, PBYTE pOutput, PDWORD pdwOutput);
__declspec(dllexport) BE_ERR	beCloseStream(HBE_STREAM hbeStream);
__declspec(dllexport) VOID		beVersion(PBE_VERSION pbeVersion);

#endif

#pragma pack(pop)

#endif
