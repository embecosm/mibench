/* nextispell.m */
/*
 *
 * Modify the code anyway you like and report changes
 * as well as any good ideas to me, willers@butp.unibe.ch
 *
 * written by Moritz Willers
 *
 */

#define DATE "4. Januar 1994\n"
#define VERSION "Version 0.4\n"

#import <appkit/appkit.h>
#import "configure.h"

#define MAXBUFLEN 1024

struct pipe_with_buf {
    int fd;
    char *buf;
};

mutex_t lock;
char misspelled[MAXBUFLEN];

@interface Dictionaire:Object
{
    int fromIspell, toIspell, fromDictionaire, toDictionaire;
}

- init;
- free;

- (BOOL)spellServer:(NXSpellServer *)sender 
	findMisspelledWord:(int *)start 
	length:(int *)length 
	inLanguage:(const char *)language
	inTextStream:(id <NXReadOnlyTextStream>)textStream 
	startingAt:(int)startPosition
	wordCount:(int *)number 
	countOnly:(BOOL)flag;
- (void)spellServer:(NXSpellServer *)sender 
	suggestGuessesForWord:(const char *)word 
	inLanguage:(const char *)language;

@end


@implementation Dictionaire

/* ********************     private functions     ************************ */

int makepipe(int *rd, int *wrt)
{
     int     piperesult, fildes[2];

     piperesult = pipe(fildes);

     *rd = fildes[0];
     *wrt = fildes[1];

     return piperesult;
}

int secure_read(int d, char *buf, int nbytes)
{

/* someday I'm going to rewrite this using the select() call instead of a nonblocking fd */

    int ret, reads = 0;
    do
    {
	ret = read(d, buf, nbytes-1);
	reads++;
    } while ((ret == -1) && (reads < 100000));
    if (reads < 100000)
    {
	return ret;
    }
    else
	fprintf(stderr, "%s: Couldn't read from pipe: %s\n", *NXArgv, strerror(errno));
    exit(1);
}

void empty_pipe(struct pipe_with_buf *pointerTopwb)
{
    int len;
    int fd = pointerTopwb->fd;
    char buf[MAXBUFLEN];
    char *bufptr;
    
    mutex_lock(lock);
    
    strcpy(buf, pointerTopwb->buf);
    
    bufptr = strrchr(buf, '\n');
    if (bufptr)
	bufptr--;
    else
	bufptr = buf;
    while (*bufptr != '\n')
    {
	len = secure_read(fd, buf, MAXBUFLEN);
	buf[len] = '\0';
	bufptr = strrchr(buf, '\n');
	if (bufptr)
	    bufptr--;
	else
	    bufptr = buf;
    }
    
    mutex_unlock(lock);
}


/* ******************************************************************** */

- init
{        
    int fdstate;
    
    [super init];
    
    lock = mutex_alloc();
    *misspelled = '\0';
    
    if (makepipe(&fromIspell,&toDictionaire)) 
    {
	fprintf(stderr, "%s: Couldn't create pipe: %s\n", *NXArgv, strerror(errno));
	[self free];
	return nil; // init wasn't successful
    }
    if (makepipe(&fromDictionaire,&toIspell))
    {
	fprintf(stderr, "%s: Couldn't create pipe: %s\n", *NXArgv, strerror(errno));
	[self free];
	return nil; // init wasn't successful
    }
    

    switch (fork())
    {
	case -1:
	    fprintf(stderr, "%s: Couldn't fork: %s\n", *NXArgv, strerror(errno));
	    [self free];
	    return nil;
	case  0:
	    close(toIspell);
	    close(fromIspell);
	    
	    if ( dup2(fromDictionaire, 0) == -1 )
		fprintf(stderr, "%s: Error establishing read pipe: %s\n", *NXArgv, strerror(errno));
	    if ( dup2(toDictionaire, 1) == -1 )
		fprintf(stderr, "%s: Error establishing write pipe: %s\n", *NXArgv, strerror(errno));
	    
	    /* change child into ispell */
	    execlp(ISPELL, NULL);
	    fprintf(stderr, "%s: Failed to exec ispellpipe: %s\n", *NXArgv, strerror(errno));
	    exit(1);
	default:
	    close(fromDictionaire);
	    close(toDictionaire);
	    	    
	    /* set fromIspell fd non blocking: */
	    fdstate = fcntl(fromIspell, F_GETFL, 0);
	    fcntl(fromIspell, F_SETFL, fdstate|O_NDELAY);
	    
#ifdef TEX
	    write(toIspell, "+\n", 2);
#endif
	    
	    break;
    }
    return self;
}

- free
{
    char eof = EOF;
    
    if (toIspell)
    {
	write(toIspell, &eof, 1);
	close(toIspell);
    }
    if (fromIspell) close(fromIspell); 
    
    return [super free];
}

/* ***********************    delegate methods    ************************* */

- (BOOL)spellServer:(NXSpellServer *)sender 
	findMisspelledWord:(int *)start 
	length:(int *)length 
	inLanguage:(const char *)language
	inTextStream:(id <NXReadOnlyTextStream>)textStream 
	startingAt:(int)startPosition
	wordCount:(int *)number 
	countOnly:(BOOL)flag
{
    char readbuf[MAXBUFLEN], writebuf[MAXBUFLEN];
    char *readbufptr, *writebufptr;
    int otherlen, len;
    int offset, linelength = 0;
    char misspelledWord[MAXBUFLEN];
    BOOL repeatLoop;
    struct pipe_with_buf pwb;
    
    if (flag)
    {
	*number = -1; /* is not able to do pure wordcounting */
	return NO;
    }
    
    if ([textStream isAtEOTS])
	return NO;
    
    mutex_lock(lock); /* to make sure the thread has emptied the pipe */
    mutex_unlock(lock);
    
    readbufptr = readbuf;
    *start = startPosition;
    
    /* set stream outside a word */
    [textStream readCharacters:readbufptr count:1];
    while (!NXIsSpace(*readbufptr) && startPosition && ![textStream isAtEOTS])
    {
	[textStream readCharacters:readbufptr count:1];
	(*start)++;
    }
    if (*readbufptr == '\n') *readbufptr = ' ';
    readbufptr++;
    len = 1;
    
    /* main loop */
    do
    {
	/* read the 80 characters form the text stream and complete the last word */
	len += [textStream readCharacters:readbufptr count:80];
	readbufptr = readbuf;
	readbufptr[len] = '\0';
	while (*readbufptr)
	{
	    if (*readbufptr == '\n')
		*readbufptr = ' ';
	    readbufptr++;
	}
	
	if (len>=80)
	    while (!(NXIsSpace(*(readbufptr-1)) || [textStream isAtEOTS]))
	    {
		[textStream readCharacters:readbufptr count:1];
		len++;
		if (*readbufptr == '\n')
		    *readbufptr = ' ';
		readbufptr++;
	    }
	*readbufptr++ = '\n';
	*readbufptr = '\0';
	linelength = len;
	len = 0;
	readbufptr = readbuf;
		

	/* send ispell the next ca. 80 chars */
	write(toIspell, "^", 1);
	while (*readbufptr)
	    write(toIspell, readbufptr++, 1);
	    
	readbufptr = readbuf;

	repeatLoop = YES;
	do
	{
	    otherlen = secure_read(fromIspell, writebuf, MAXBUFLEN);
	    writebuf[otherlen] = '\0';
	    writebufptr = writebuf;
	    while (writebufptr && *writebufptr)
	    {
		/* make sure a whole line is ready to be processed */
		while (!strchr(writebufptr, '\n'))
		{
		    /* add more to the buffer */
		    strcpy(writebuf, writebufptr);
		    writebufptr = strchr(writebuf, '\0');
		    otherlen = secure_read(fromIspell, writebufptr, MAXBUFLEN - strlen(writebuf));
		    writebufptr[otherlen] = '\0';
		    writebufptr = writebuf;
		}
		/* then process the line: */
		switch(*writebufptr)
		{
		    case '*':
		    case '+':
		    case '-':
			(*number)++;
			break;
		    case '&':
		    case '?':
			strcpy(misspelled, writebufptr);
			writebufptr += 2;
			sscanf(writebufptr, "%s %*d %d", misspelledWord, &offset);
			if ([sender isInUserDictionary:(const char *)misspelledWord caseSensitive:NO])
			{
			    (*number)++;
			    break;
			}
			*length = strlen(misspelledWord);
			*start += offset - 1;
			
			pwb.fd = fromIspell;
			pwb.buf = writebuf;
			cthread_detach(cthread_fork( (cthread_fn_t)empty_pipe, (any_t)&pwb));
			return YES;
		    case '#':
			strcpy(misspelled, writebufptr);
			writebufptr += 2;
			sscanf(writebufptr, "%s %d", misspelledWord, &offset);
			if ([sender isInUserDictionary:(const char *)misspelledWord caseSensitive:NO])
			{
			    (*number)++;
			    break;
			}
			*length = strlen(misspelledWord);
			*start += offset - 1; 
			pwb.fd = fromIspell;
			pwb.buf = writebuf;
			cthread_detach(cthread_fork( (cthread_fn_t)empty_pipe, (any_t)&pwb));
			return YES;
		    case '\n':
			*start += linelength;
			linelength = 0;
			repeatLoop = NO;
		    default:
			break;
		}
		writebufptr = strchr(writebufptr, '\n');
		if (writebufptr)
		    writebufptr++;
	    }
	} while (repeatLoop);
    }
    while (![textStream isAtEOTS]);

        
    return NO; /* no misspelled words found */
}

- (void)spellServer:(NXSpellServer *)sender 
	suggestGuessesForWord:(const char *)word 
	inLanguage:(const char *)language
{
    int len, misscount;
    char buf[MAXBUFLEN];
    char *bufptr, *guess;
    
    if (*misspelled)
    {
	bufptr = strchr(misspelled, '\n');
	bufptr++;
	*bufptr = '\0';
	switch (*misspelled)
	{
	    case '&':
	    case '?':
		bufptr = strchr(misspelled + 2, ' ') + 1;
		misscount = atoi(bufptr);
		bufptr = strchr(misspelled, ':');
		while (bufptr != NULL  &&  misscount > 0)
		{
		    misscount--;
		    guess = bufptr + 2;
		    if (bufptr = strchr(guess, ','))
			*bufptr = '\0';
		    else
		    {
			bufptr = strchr(guess, '\n');
			*bufptr = '\0';
			bufptr = NULL;
		    }
		    [sender addGuess:guess];
		}
	    case '#':
		; /* no guesses */
	}
	*misspelled = '\0';
	return;
    } /* else */
    
    mutex_lock(lock); /* make sure that the pipe has been emptied */
    mutex_unlock(lock);
    
    write(toIspell, "^", 1);
    write(toIspell, word, strlen(word));
    write(toIspell, "\n", 1);
    
    bufptr = buf;
    *buf = '\0';
    do
    {
	len = secure_read(fromIspell, bufptr, MAXBUFLEN - strlen(buf));
	bufptr[len] = '\0';
	if (strchr(buf, '\n') == strrchr(buf, '\n'))
	    bufptr = strchr(buf, '\0');
	else
	    bufptr = buf;
    } while (!(*bufptr));
    
    switch (*bufptr)
    {
	case '*':
	case '+':
	case '-':
	    [sender addGuess:word];
	    break;
	case '&':
	case '?':
	    bufptr = strchr(bufptr + 2, ' ') + 1;
	    misscount = atoi(bufptr);
	    bufptr = strchr(bufptr, ':');
	    while (bufptr != NULL  &&  misscount > 0)
	    {
		misscount--;
		guess = bufptr + 2;
		if (bufptr = strchr(guess, ','))
		    *bufptr = '\0';
		else
		{
		    bufptr = strchr(guess, '\n');
		    *bufptr = '\0';
		    bufptr = NULL;
		}
		[sender addGuess:guess];
	    }
	case '#':
	    ; /* no guesses */
    }
    
    return;
}

@end

/* **************************     main     ******************************* */

void main(int argc, char **argv)
{
    NXSpellServer *aServer;
    
    if (argc > 1)
    {
	if (!strcmp((argv[1]), "-v"))
	{
	    printf("nextispell by Moritz Willers\n");
	    printf("email: willers@butp.unibe.ch (NeXTMail)\n");
	    printf(VERSION);
	    printf(DATE);
	    exit(0);
	} else
	{
	    fprintf(stderr, "Usage: %s [-v]\n", *argv);
	    exit(0);
	}
    }
    aServer = [[NXSpellServer alloc] init];
    if ([aServer registerLanguage:LANGUAGE byVendor:VENDOR]) {
	[aServer setDelegate:[[Dictionaire alloc] init]];
	[aServer run];
	fprintf(stderr, "Unexpected death of %s!\n", *argv);
    } else {
	fprintf(stderr, "Unable to check in %s.\n", *argv);
    }
}
