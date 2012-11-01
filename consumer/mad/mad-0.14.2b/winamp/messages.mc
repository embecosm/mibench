; /* -*- Fundamental -*- */

SeverityNames=(Success=0x0:STATUS_SEVERITY_SUCCESS
               Informational=0x1:STATUS_SEVERITY_INFORMATIONAL
               Warning=0x2:STATUS_SEVERITY_WARNING
               Error=0x3:STATUS_SEVERITY_ERROR)

; /* Warnings */

MessageId=
Severity=Warning
SymbolicName=IDS_WARN_NOHEADER
Language=English
No MPEG audio header was found.
.

MessageId=
SymbolicName=IDS_WARN_BADTRACK
Language=English
Please enter a track number from 1-255.
.

MessageId=
SymbolicName=IDS_WARN_COMMENTTOOLONG
Language=English
When a track number is specified, the comment cannot exceed 28 characters.
.

; /* Errors */

MessageId=
Severity=Error
SymbolicName=IDS_ERR_ID3V1WRITE
Language=English
The ID3v1 tag was not completely written.
.

MessageId=
SymbolicName=IDS_ERR_NOMEM
Language=English
There is not enough memory to proceed.
.

MessageId=
SymbolicName=IDS_ERR_ID3FAIL
Language=English
The ID3 tag(s) could not be read.
.
