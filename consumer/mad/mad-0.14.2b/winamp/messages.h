 /* -*- Fundamental -*- */
 /* Warnings */
//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//


//
// Define the severity codes
//
#define STATUS_SEVERITY_WARNING          0x2
#define STATUS_SEVERITY_SUCCESS          0x0
#define STATUS_SEVERITY_INFORMATIONAL    0x1
#define STATUS_SEVERITY_ERROR            0x3


//
// MessageId: IDS_WARN_NOHEADER
//
// MessageText:
//
//  No MPEG audio header was found.
//
#define IDS_WARN_NOHEADER                0xA0000001L

//
// MessageId: IDS_WARN_BADTRACK
//
// MessageText:
//
//  Please enter a track number from 1-255.
//
#define IDS_WARN_BADTRACK                0xA0000002L

//
// MessageId: IDS_WARN_COMMENTTOOLONG
//
// MessageText:
//
//  When a track number is specified, the comment cannot exceed 28 characters.
//
#define IDS_WARN_COMMENTTOOLONG          0xA0000003L

 /* Errors */
//
// MessageId: IDS_ERR_ID3V1WRITE
//
// MessageText:
//
//  The ID3v1 tag was not completely written.
//
#define IDS_ERR_ID3V1WRITE               0xE0000004L

//
// MessageId: IDS_ERR_NOMEM
//
// MessageText:
//
//  There is not enough memory to proceed.
//
#define IDS_ERR_NOMEM                    0xE0000005L

//
// MessageId: IDS_ERR_ID3FAIL
//
// MessageText:
//
//  The ID3 tag(s) could not be read.
//
#define IDS_ERR_ID3FAIL                  0xE0000006L

