$ ! OpenVMS command file to emulate behavior of:
$ !
$ !     Define the command for copying one file to another
$ !
$ COPY="COPY/LOG"
$ FILE = F$SEARCH("''P1'")
$ IF P2 .EQS. "." THEN P2 = "[]"
$ IF "''FILE'" .NES. "" THEN COPY 'FILE' 'P2'
