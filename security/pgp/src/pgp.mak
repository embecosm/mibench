.AUTODEPEND

#		*Translator Definitions*
CC = bcc +PGP.CFG
TASM = TASM
TLINK = tlink


#		*Implicit Rules*
.c.obj:
  $(CC) -c {$< }

.cpp.obj:
  $(CC) -c {$< }

#		*List Macros*


EXE_dependencies =  \
  charset.obj \
  config.obj \
  crypto.obj \
  fileio.obj \
  genprime.obj \
  getopt.obj \
  idea.obj \
  keyadd.obj \
  keymaint.obj \
  keymgmt.obj \
  language.obj \
  md5.obj \
  mdfile.obj \
  more.obj \
  mpiio.obj \
  mpilib.obj \
  passwd.obj \
  armor.obj \
  pgp.obj \
  random.obj \
  rsagen.obj \
  system.obj \
  zbits.obj \
  zdeflate.obj \
  zfile_io.obj \
  zglobals.obj \
  zinflate.obj \
  zip.obj \
  zipup.obj \
  ztrees.obj \
  zunzip.obj \
  8086.obj \
  zmatch.obj

#		*Explicit Rules*
pgp.exe: pgp.cfg $(EXE_dependencies)
  $(TLINK) /x/c/P-/L\BCC\LIB @&&|
c0l.obj+
charset.obj+
config.obj+
crypto.obj+
fileio.obj+
genprime.obj+
getopt.obj+
idea.obj+
keyadd.obj+
keymaint.obj+
keymgmt.obj+
language.obj+
md5.obj+
mdfile.obj+
more.obj+
mpiio.obj+
mpilib.obj+
passwd.obj+
armor.obj+
pgp.obj+
random.obj+
rsagen.obj+
system.obj+
zbits.obj+
zdeflate.obj+
zfile_io.obj+
zglobals.obj+
zinflate.obj+
zip.obj+
zipup.obj+
ztrees.obj+
zunzip.obj+
8086.obj+
zmatch.obj
pgp
		# no map file
cl.lib
|


#		*Individual File Dependencies*
charset.obj: charset.c 

config.obj: config.c 

crypto.obj: crypto.c 

fileio.obj: fileio.c 

genprime.obj: genprime.c 

getopt.obj: getopt.c 

idea.obj: idea.c 

keyadd.obj: keyadd.c 

keymaint.obj: keymaint.c 

keymgmt.obj: keymgmt.c 

language.obj: language.c 

md5.obj: md5.c 

mdfile.obj: mdfile.c 

more.obj: more.c 

mpiio.obj: mpiio.c 

mpilib.obj: mpilib.c 

passwd.obj: passwd.c 

armor.obj: armor.c 

pgp.obj: pgp.c 

random.obj: random.c 

rsagen.obj: rsagen.c 

system.obj: system.c 

zbits.obj: zbits.c 

zdeflate.obj: zdeflate.c 

zfile_io.obj: zfile_io.c 

zglobals.obj: zglobals.c 

zinflate.obj: zinflate.c 

zip.obj: zip.c 

zipup.obj: zipup.c 

ztrees.obj: ztrees.c 

zunzip.obj: zunzip.c 

8086.obj: 8086.asm 
	$(TASM) /MX /ZI /O 8086.ASM,8086.OBJ

zmatch.obj: zmatch.asm 
	$(TASM) /DDYN_ALLOC=1 /DSS_NEQ_DS=1 /MX /ZI /O ZMATCH.ASM,ZMATCH.OBJ

#		*Compiler Configuration File*
pgp.cfg: pgp.mak
  copy &&|
-ml
-f-
-G
-O
-Z
-h
-wamb
-wamp
-wasm
-wpro
-wdef
-wnod
-wstv
-wuse
-I\BCC\INCLUDE
-L\BCC\LIB
-DMSDOS;DYN_ALLOC;SMALL_MEM
-P-.C
-Ff
| pgp.cfg


