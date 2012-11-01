/* usuals.h - The usual typedefs, etc.
*/
#ifndef USUALS /* Assures no redefinitions of usual types...*/
#define USUALS

typedef unsigned char boolean;	/* values are TRUE or FALSE */
typedef unsigned char byte;	/* values are 0-255 */
typedef byte *byteptr;	/* pointer to byte */
typedef char *string;	/* pointer to ASCII character string */
typedef unsigned short word16;	/* values are 0-65535 */
typedef unsigned long word32;	/* values are 0-4294967295 */

#ifndef TRUE
#define FALSE 0
#define TRUE (!FALSE)
#endif	/* if TRUE not already defined */

#ifndef min	/* if min macro not already defined */
#define min(a,b) ( (a)<(b) ? (a) : (b) )
#define max(a,b) ( (a)>(b) ? (a) : (b) )
#endif	/* if min macro not already defined */

#endif	/* if USUALS not already defined */

