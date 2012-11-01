/*
 *	Blade DLL Interface for LAME.
 *
 *	Copyright (c) 1999 A.L. Faber
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "BladeMP3EncDLL.h"
#include <assert.h>
#include "version.h"  
#include "VbrTag.h"   
#include "lame.h"
/*
#include "get_audio.h"
#include "globalflags.h"
#include "machine.h"
#include "util.h"
*/

#ifdef _DEBUG
	#define _DEBUGDLL 1
#endif


const int MAJORVERSION=1;
const int MINORVERSION=05;
const int CURRENT_STRUCT_VERSION=1;
const int CURRENT_STRUCT_SIZE=sizeof(BE_CONFIG);





// Local variables
static int		nPsychoModel=2;
static BOOL		bFirstFrame=TRUE;
static DWORD	dwSampleBufferSize=0;


#ifdef _DEBUGDLL
void dump_config( char *inPath, char *outPath);
#endif

lame_global_flags gf;

static void InitParams()
{
    bFirstFrame=TRUE;
    lame_init(&gf);

}




__declspec(dllexport) BE_ERR	beInitStream(PBE_CONFIG pbeConfig, PDWORD dwSamples, PDWORD dwBufferSize, PHBE_STREAM phbeStream)
{
	int			nDllArgC=0;
	BE_CONFIG	lameConfig;

	InitParams();

	// clear out structure
	memset(&lameConfig,0x00,CURRENT_STRUCT_SIZE);

	// Check if this is a regular BLADE_ENCODER header
	if (pbeConfig->dwConfig!=BE_CONFIG_LAME)
	{
		int	nCRC=pbeConfig->format.mp3.bCRC;
		int nVBR=(nCRC>>12)&0x0F;

		// Copy parameter from old Blade structure
		lameConfig.format.LHV1.dwSampleRate	=pbeConfig->format.mp3.dwSampleRate;
		//for low bitrates, LAME will automatically downsample for better
		//sound quality.  Forcing output samplerate = input samplerate is not a good idea 
		//unless the user specifically requests it:
		//lameConfig.format.LHV1.dwReSampleRate=pbeConfig->format.mp3.dwSampleRate;
		lameConfig.format.LHV1.nMode		=(pbeConfig->format.mp3.byMode&0x0F);
		lameConfig.format.LHV1.dwBitrate	=pbeConfig->format.mp3.wBitrate;
		lameConfig.format.LHV1.bPrivate		=pbeConfig->format.mp3.bPrivate;
		lameConfig.format.LHV1.bOriginal	=pbeConfig->format.mp3.bOriginal;
		lameConfig.format.LHV1.bCRC			=nCRC&0x01;
		lameConfig.format.LHV1.bCopyright	=pbeConfig->format.mp3.bCopyright;
	
		// Fill out the unknowns
		lameConfig.format.LHV1.dwStructSize=CURRENT_STRUCT_VERSION;
		lameConfig.format.LHV1.dwStructVersion=CURRENT_STRUCT_SIZE;

		// Get VBR setting from fourth nibble
		if (nVBR>0)
		{
			lameConfig.format.LHV1.bWriteVBRHeader=TRUE;
			lameConfig.format.LHV1.bEnableVBR=TRUE;
			lameConfig.format.LHV1.nVBRQuality=nVBR-1;
		}

		// Get Quality from third nibble
		lameConfig.format.LHV1.nQuality=(MPEG_QUALITY)((nCRC>>8)&0x0F);

	}
	else
	{
		// Copy the parameters
		memcpy(&lameConfig,pbeConfig,pbeConfig->format.LHV1.dwStructSize);
	}


	// Not used, always assign stream 1
	*phbeStream=1;



	// --------------- Set arguments to LAME encoder -------------------------

	// Set zero argument, the filename
	//strcpy(DllArgV[nDllArgC++],"LameDLLEncoder");

  	switch (lameConfig.format.LHV1.nMode)
	{
		case BE_MP3_MODE_STEREO:
			gf.mode=0;
			gf.mode_fixed=1;  /* dont allow LAME to change the mode */
			gf.num_channels=2;
		break;
		case BE_MP3_MODE_JSTEREO:
			gf.mode=1;
			gf.mode_fixed=1;
			gf.num_channels=2;
		break;
		case BE_MP3_MODE_MONO:
			gf.mode=3;
			gf.mode_fixed=1;
			gf.num_channels=1;
		break;
		case BE_MP3_MODE_DUALCHANNEL:
			gf.force_ms=1;
			gf.mode=1;
			gf.mode_fixed=1;
			gf.num_channels=2;
		break;
		default:
		{
			char lpszError[255];
			sprintf(lpszError,"Invalid lameConfig.format.LHV1.nMode, value is %d\n",lameConfig.format.LHV1.nMode);
			OutputDebugString(lpszError);
			return BE_ERR_INVALID_FORMAT_PARAMETERS;
		}
	}

	switch (lameConfig.format.LHV1.nQuality)
	{
		case NORMAL_QUALITY:	// Nothing special
			break;
		case LOW_QUALITY:		// -f flag
			gf.quality=9;
			break;
		case HIGH_QUALITY:		// -h flag for high qualtiy
			gf.quality=2;
			break;
		case VOICE_QUALITY:		// --voice flag for experimental voice mode
			gf.lowpassfreq=12000;
			gf.VBR_max_bitrate_kbps=160;
			gf.no_short_blocks=1;
		break;
	}

	if (lameConfig.format.LHV1.bEnableVBR)
	{
		// 0=no vbr 1..10 is VBR quality setting -1
		gf.VBR=1;
		gf.VBR_q=lameConfig.format.LHV1.nVBRQuality;
	}

	// Set frequency
	gf.in_samplerate=lameConfig.format.LHV1.dwSampleRate;

	// Set frequency resampling rate, if specified
	if (lameConfig.format.LHV1.dwReSampleRate>0)
		gf.out_samplerate=lameConfig.format.LHV1.dwReSampleRate;
		
	
	// Set bitrate.  (CDex users always specify bitrate=Min bitrate when using VBR)
	gf.brate=lameConfig.format.LHV1.dwBitrate;
	gf.VBR_min_bitrate_kbps=gf.brate;
			
	// Set Maxbitrate, if specified
	if (lameConfig.format.LHV1.dwMaxBitrate>0)
		gf.VBR_max_bitrate_kbps=lameConfig.format.LHV1.dwMaxBitrate;
	
	// Set copyright flag?
	if (lameConfig.format.LHV1.bCopyright)
		gf.copyright=1;

	// Do we have to tag  it as non original 
	if (!lameConfig.format.LHV1.bOriginal)
		gf.original=0;

	// Add CRC?
	if (lameConfig.format.LHV1.bCRC)
		gf.error_protection=1;

	gf.silent=1;  /* disable status ouput */

	// Set private bit?
	if (lameConfig.format.LHV1.bPrivate)
	{
		gf.extension = 0;
	}
	else
	{
		gf.extension = 1;
	}
	
	lame_init_params(&gf);	

	//LAME encoding call will accept any number of samples.  
	*dwSamples=1152*gf.num_channels;

	// Set the input sample buffer size, so we know what we can expect
	dwSampleBufferSize=*dwSamples;

	// Set MP3 buffer size
	// conservative estimate
	*dwBufferSize=1.25*(*dwSamples/gf.num_channels) + 7200;


#ifdef _DEBUGDLL
	dump_config(gf.inPath,gf.outPath);
#endif

	// Everything went OK, thus return SUCCESSFUL
	return BE_ERR_SUCCESSFUL;
}



__declspec(dllexport) BE_ERR	beDeinitStream(HBE_STREAM hbeStream, PBYTE pOutput, PDWORD pdwOutput)
{

	*pdwOutput = lame_encode_finish(&gf,pOutput,0);

	if (*pdwOutput<0) {
		*pdwOutput=0;
		return BE_ERR_BUFFER_TOO_SMALL;
	}

	return BE_ERR_SUCCESSFUL;
}


__declspec(dllexport) BE_ERR	beCloseStream(HBE_STREAM hbeStream)
{
	// DeInit encoder
//	return DeInitEncoder();
	return BE_ERR_SUCCESSFUL;
}



__declspec(dllexport) VOID		beVersion(PBE_VERSION pbeVersion)
{
	// DLL Release date
	char lpszDate[20];
	char lpszTemp[5];


	// Set DLL interface version
	pbeVersion->byDLLMajorVersion=MAJORVERSION;
	pbeVersion->byDLLMinorVersion=MINORVERSION;

	// Set Engine version number (Same as Lame version)
	pbeVersion->byMajorVersion=LAME_MAJOR_VERSION;
	pbeVersion->byMinorVersion=LAME_MINOR_VERSION;

	// Get compilation date
	strcpy(lpszDate,__DATE__);

	// Get the first three character, which is the month
	strncpy(lpszTemp,lpszDate,3);

	// Set month
	if (strcmp(lpszTemp,"Jan")==0)	pbeVersion->byMonth=1;
	if (strcmp(lpszTemp,"Feb")==0)	pbeVersion->byMonth=2;
	if (strcmp(lpszTemp,"Mar")==0)	pbeVersion->byMonth=3;
	if (strcmp(lpszTemp,"Apr")==0)	pbeVersion->byMonth=4;
	if (strcmp(lpszTemp,"May")==0)	pbeVersion->byMonth=5;
	if (strcmp(lpszTemp,"Jun")==0)	pbeVersion->byMonth=6;
	if (strcmp(lpszTemp,"Jul")==0)	pbeVersion->byMonth=7;
	if (strcmp(lpszTemp,"Aug")==0)	pbeVersion->byMonth=8;
	if (strcmp(lpszTemp,"Sep")==0)	pbeVersion->byMonth=9;
	if (strcmp(lpszTemp,"Oct")==0)	pbeVersion->byMonth=10;
	if (strcmp(lpszTemp,"Nov")==0)	pbeVersion->byMonth=11;
	if (strcmp(lpszTemp,"Dec")==0)	pbeVersion->byMonth=12;

	// Get day of month string (char [4..5])
	pbeVersion->byDay=atoi(lpszDate+4);

	// Get year of compilation date (char [7..10])
	pbeVersion->wYear=atoi(lpszDate+7);

	memset(pbeVersion->zHomepage,0x00,BE_MAX_HOMEPAGE);

	strcpy(pbeVersion->zHomepage,"http://www.sulaco.org/mp3/");
}

__declspec(dllexport) BE_ERR	beEncodeChunk(HBE_STREAM hbeStream, DWORD nSamples, 
			 PSHORT pSamples, PBYTE pOutput, PDWORD pdwOutput)
{

	// Encode it
        int dwSamples;
	dwSamples=nSamples/gf.num_channels;

	// old versions of lame_enc.dll required exactly 1152 samples
	// and worked even if nSamples accidently set to 2304
	// simulate this behavoir:
	if (gf.num_channels==1 && nSamples == 2304)
	  dwSamples/=2;

	*pdwOutput=lame_encode_buffer_interleaved(&gf,pSamples,dwSamples,pOutput,0);


	if (*pdwOutput<0) {
		*pdwOutput=0;
		return BE_ERR_BUFFER_TOO_SMALL;
	}

	return BE_ERR_SUCCESSFUL;
}


__declspec(dllexport) BE_ERR beWriteVBRHeader(LPCSTR lpszFileName)
{
	if (gf.bWriteVbrTag)
	{
		// Calculate relative quality of VBR stream 
		// 0=best, 100=worst
		int nQuality=gf.VBR_q*100/9;

		// Write Xing header again
		return PutVbrTag((LPSTR)lpszFileName,nQuality,1-gf.version);
	}
	return BE_ERR_INVALID_FORMAT_PARAMETERS;
}


BOOL APIENTRY DllMain(HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved)
{
    switch( ul_reason_for_call )
	{
		case DLL_PROCESS_ATTACH:
#ifdef _DEBUGDLL
			OutputDebugString("Attach Process \n");
#endif
		break;
		case DLL_THREAD_ATTACH:
#ifdef _DEBUGDLL
			OutputDebugString("Attach Thread \n");
#endif
		break;
		case DLL_THREAD_DETACH:
#ifdef _DEBUGDLL
			OutputDebugString("Detach Thread \n");
#endif
		break;
		case DLL_PROCESS_DETACH:
#ifdef _DEBUGDLL
			OutputDebugString("Detach Process \n");
#endif
		break;
    }
    return TRUE;
}


#ifdef _DEBUGDLL
void dump_config( char *inPath, char *outPath)
{
  	char strTmp[255];

	OutputDebugString("Encoding configuration:\n");


	sprintf(strTmp,"Write VBR Header=%s\n",(gf.bWriteVbrTag)?"Yes":"No");
	OutputDebugString(strTmp);

	sprintf(strTmp,"version=%d\n",gf.version);
	OutputDebugString(strTmp);


	sprintf(strTmp,"Layer=3   mode=%d  \n",gf.mode);
	OutputDebugString(strTmp);


	sprintf(strTmp,"samp frq=%.1f kHz   total bitrate=%d kbps\n",gf.in_samplerate/1000.0);
	OutputDebugString(strTmp);

	sprintf(strTmp,"de-emph=%d   c/right=%d   orig=%d   errprot=%s\n",gf.emphasis, gf.copyright, gf.original,((gf.error_protection) ? "on" : "off"));
	OutputDebugString(strTmp);

//	sprintf(strTmp,"16 Khz cut off is %s\n",(0)?"enabled":"disabled");
//	OutputDebugString(strTmp);

	sprintf(strTmp,"Fast mode is %s\n",(gf.quality==9)?"enabled":"disabled");
	OutputDebugString(strTmp);

	sprintf(strTmp,"Force ms %s\n",(gf.force_ms)?"enabled":"disabled");
	OutputDebugString(strTmp);

//	sprintf(strTmp,"GPsycho acoustic model is %s\n",(gpsycho)?"enabled":"disabled");
//	OutputDebugString(strTmp);

	sprintf(strTmp,"VRB is %s, VBR_q value is  %d\n",(gf.VBR)?"enabled":"disabled",gf.VBR_q);
	OutputDebugString(strTmp);

	sprintf(strTmp,"input file: '%s'   output file: '%s'\n", inPath, outPath);
	OutputDebugString(strTmp);

//	sprintf(strTmp,"Voice mode %s\n",(voice_mode)?"enabled":"disabled");
//	OutputDebugString(strTmp);

	sprintf(strTmp,"Encoding as %.1f kHz %d kbps %d MPEG-%d LayerIII file\n",gf.out_samplerate/1000.0,gf.brate,gf.mode,3 - gf.mode_gr);
	OutputDebugString(strTmp);
}


void DispErr(LPSTR strErr)
{
	MessageBox(NULL,strErr,"",MB_OK);
}

#endif
