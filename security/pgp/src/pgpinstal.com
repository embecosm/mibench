$!x='f$ver(0)'
$!
$ pgpinsver="1.0i"
$ pgpver="263i"
$ pgpver_human ="2.6.3i"
$!
$!Program:	PGPINSTAL.COM V1.0i for PGP 2.6.3i
$!
$!Description:
$!	Given the presence of appropriately named & files,
$!	this procedure will attempt to construct a VMS executable
$!	called PGP.EXE.
$!
$!Instructions:
$!	1) Type "@PGPINSTAL" and answer the questions (do NOT type the quotes)
$!
$!	Note: If you know that the default answers are correct, then you can
$!	  simply type "@PGPINSTAL 1" and the procedure will start
$!	  directly
$!
$!End-User notes:
$!	* YOU MUST BE USING VAXC or DECC!
$!	* YOU MUST BE BUILDING PGP 2.6.3i from the source distribution!
$!	* The debug & optimize flags are in a line beginning 'dbg='... search
$!	  for this & modify as needed.  The default distribution is /nodebug
$!	  and /opt=noinline (due to an _old_ vaxc optimizer bug...)
$!	* Note that md5.c is an exception under VAX/DECC - must be /noopt or
$!	  the compiler will hang :(
$!	* if any {p1..p4} is nonnull then an immediate install will be attempted
$!	* if p1 is nonnull, then p1 is assumed to be the location of pgp
$!	  distribution & all other values default if possible.
$!	* if p2 is nonnull, then p2 is the default UNZIP definition
$!	* if p3 is nonnull, then p3 is the default dir for top-level
$!	* if p4 is nonnull, then p4 is the default top-level name
$!
$!Implementation notes:
$!	* There are some lines of code present in this module that are
$!	  disabled, but they're useful reference material, so they got
$!	  left here.
$!	* if p1 is of the form '@label', then at top of build procedure,
$!	  a jump is performed to the requested label to bypass portions
$!	  of the build (needed for relinking, for example)
$!	    o **This is NOT intended to be a supported function!**
$!	    o P2 now must be nonnull to force immediate mode
$!	    o This option is for debugging purposes only and most of the
$!	      start/stop pairs are not in place, but may be added. You
$!	      should read & understand the DCL if you want to add these.
$!	    o The $LNK_START entrypoint can be used to re-link
$!
$ jumpto = f$elem(1,"@","''p1'")
$ jump = jumpto.nes."@"
$ if jump then p1=""
$ immediate = ("''p1'"+"''p2'"+"''p3'"+"''p4'").nes.""
$ if jump then p2=""
$ if jump.and.immediate then p1="sys$login:login.com"
$!
$restart_install:
$ on control_c then goto halt
$ on control_y then goto halt
$ say="write sys$output"
$ ask="call ask_user"
$ edit="edit"
$ delete="delete"
$ start_dir = f$environment("default")
$ call get_hw_model
$ hw_model = f$int(f$trnlnm("hw_model"))
$ deassign hw_model
$ if hw_model .ge. 1024
$ then
    say "%PGPINSTAL-I-ALPHA, Target architecture is ALPHA/VMS"
$   macopts="/migrate/nowarn/nodeb/opt"
$   lnkopts="/notrace/map"
$   cc="cc/stan=vaxc/nomember_align"
$   cdefs=""
$   alpha=1
$   vax=0
$   archltr="a"
$   mach="Alpha"
$   mach2="n Alpha"
$   estspace=11000
$ else
    say "%PGPINSTAL-I-VAX, Target architecture is VAX/VMS"
$   macopts=""
$   lnkopts="/notrace/map"
$   cc="cc"
$   cdefs=""
$   alpha=0
$   vax=1
$   archltr="v"
$   mach="VAX"
$   mach2=" VAX"
$   estspace=8500
$ endif
$ wait 0:0:2 !Long enough to see target message
$ goto start_install
$!
$get_hw_model: subroutine
$ mess=f$environment("message")
$ set mess/nofacility/noident/noseverity/notext
$ x=0
$ set noon
$ x=f$getsyi("hw_model")
$ set mess 'mess'
$ define hw_model 'x'
$ endsubroutine
$!
$ask_user: subroutine !p1=question, p2=lognam_to_define, p3=default
$ on control_c then goto bomb
$ on control_y then goto bomb
$retry_input:
$ if "''p3'".nes.""
$ then
$   if immediate
$   then
$     line="''p3'"
$   else
$     read/end=bomb/prompt="''p1'? [''p3'] " sys$command line
$   endif
$ else
$   if immediate
$   then
$     say "%PGPINSTAL-E-NOIMMED, Immediate installation failed"
$     say "%PGPINSTAL-F-INPREQD, Input is required for /''p1'/"
$     exit 20
$   endif
$   read/end=bomb/prompt="''p1'? " sys$command line
$ endif
$ if "''p3'".nes."" then if "''line'".eqs."" then line=p3
$ if "''line'".eqs."" then -
  say "%PGPINSTAL-E-INPREQD, Input is required here - please retry"
$ if "''line'".eqs."" then -
  goto retry_input
$ define/nolog 'p2' "''line'"
$ exit 1
$bomb:
$!x='f$ver(0)'
$ say "%PGPINSTAL-F-INSABORT, Installation has been aborted"
$ exit 2160+%x10000000
$ endsubroutine
$halt:
$!x='f$ver(0)'
$ say "%PGPINSTAL-F-INSABORT, Installation has been aborted"
$ exit 2160+%x10000000
$start_install:
$ if immediate then goto skip_lecture
$ say "[2J[H"
$ say "Hello"
$ say ""
$ say "Welcome to the PGP ''pgpver_human' automated build procedure for OpenVMS."
$ say "You seem to be running on a''mach2'... Parameters will be adjusted accordingly."
$ say ""
$ say "There are several prerequisites for building PGP ''pgpver_human' on your ''mach':
$ say "	1) A''mach2' (which you already appear to have...)"
$ say "	2) A VAXC or DECC compiler & A license*"
$ say "	3) Time & disk space (about ''estspace' blocks)"
$ say ""
$ say "If you do not have one or more of these items, then now would be a good"
$ say "time to either retrieve whatever is missing, or stop this installation."
$ say ""
$ say "*This procedure allows substitution - you will be prompted and allowed to"
$ say " re-specify the cc command definition."
$ say ""
$ say "^Z terminates when expecting input... ^Y aborts the procedure"
$ read/end=halt/prompt="[24HPress return..." sys$command junk
$ say "[2J[H"
$ say ""
$ say "You will be asked the following questions... please take a moment and"
$ say "ascertain the answers to these before continuing.  It will make life at"
$ say "least a tiny bit easier."
$ say ""
$ say "	1) What should the CC command be defined as?"
$ say ""
$ say "This will be defaulted if possible."
$ say ""
$ read/end=halt/prompt="[24HPress return..." sys$command junk
$ say "[2J[H"
$ say "Please note any errors you receive running this procedure.  I am trying"
$ say "to keep this procedure up to date with the latest release of PGP, and"
$ say "I'm also fairly sure that there are situations possible where this procedure"
$ say "may not operate correctly."
$ say ""
$ say "If you do get errors - please make certain of a few things:"
$ say "  1) That you are using VAXC or DECC as opposed to GNUC"
$ say "  2) That your PGP''pgpver' distribution has not been tampered with"
$ say ""
$ read/end=halt/prompt="[24HPress return..." sys$command junk
$skip_lecture:
$!
$!Sniff out the environment & ask the user the questions
$!
$ if .not.immediate then -
  say "[2J[H"
$!
$!Look for VAXC/DECC - Allow user to override... maybe gnu works for all I know
$ vaxc="N"
$ ccname=""
$ if alpha then goto skip_vaxc
$ if (f$search("vaxc").nes."") .or. -
     (f$search("sys$system:vaxc.exe").nes."")
$ then
$   vaxc="Y"
$   ccname="VAXC"
$   say "%PGPINSTAL-I-VAXC, VAXC.EXE located - defaulting to VAXC"
$ endif
$ decc="N"
$ if vaxc then goto skip_decc
$skip_vaxc:
$ decc="N"
$ if (f$search("decc$compiler").nes."") .or. -
     (f$search("sys$system:decc$compiler.exe").nes."")
$ then
$   decc="Y"
$   ccname="DECC"
$   if cc.eqs."cc" then cc="cc/stan=vaxc"
$   say "%PGPINSTAL-I-DECC, DECC$COMPILER located - defaulting to DECC"
$ endif
$skip_decc:
$ if .not.(DECC .or. VAXC)
$ then
$   say "%PGPINSTAL-W-NODEFCC, No default C compiler located"
$  !Just assume a generic equivalence based on platform architecture
$   ccname="Generic_VAXC"
$   if alpha then ccname="Generic_DECC"
$ endif
$!
$!Modify compiler definition
$ say "%PGPINSTAL-I-CURRCC, Current CC commpiler is ""''ccname'"""
$ say "%PGPINSTAL-I-CCDEF, CC command is ""''cc'"""
$ ask "Define cc as" "newcc" "''cc'"
$ if .not.f$int($status) then exit '$status
$ cc=f$trnlnm("newcc")
$ deassign newcc
$!
$ say "%PGPINSTAL-I-CCDEF, 'CC' is defined as ''cc'"
$re_ask1:
$ if jump.and.immediate then goto perform_installation
$ ask "Is this acceptable (Y/N)" "answer" "Y"
$ if .not.f$int($status) then exit '$status
$ answer = f$edit(f$trnlnm("answer"),"upcase,collapse")
$ deassign answer
$ if answer.eqs."Y" then goto perform_installation
$ if answer.nes."N" then goto re_ask1
$re_ask2:
$ ask "Do you want to terminate installation (Y/N)" "answer" "Y"
$ if .not.f$int($status) then exit '$status
$ answer = f$edit(f$trnlnm("answer"),"upcase,collapse")
$ deassign answer
$ if answer.eqs."Y" then goto terminate_installation
$ if answer.nes."N" then goto re_ask2
$ say "%PGPINSTAL-I-RESTART, Installation process is restarting"
$ read/end=halt/prompt="[24HPress return..." sys$command junk
$ immediate=0  !Disable for restart - just in case we ever get here
$ goto restart_install
$!
$terminate_installation:
$ say "%PGPINSTAL-F-TERMINATED, Installation terminated"
$ set def 'start_dir'
$ exit 44+%x10000000
$!
$perform_installation:
$ this_file = f$environment("procedure") - "]["
$ this_dir = f$element(0,"]",this_file)+"]"
$ set def 'this_dir'
$ src = "''this_dir'"
$ rsa = "''this_dir'"
$ obj = "sys$disk:[]"
$!
$start_build:
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!@rsabuild
$!rsabuild:
$!!src="[-.rsaref.source]"
$!!obj="sys$disk:[]"
$ sp_cflgs = ""
$ if vax.and.decc then sp_cflgs = "/PREFIX=EXCEPT=(optind,optarg,optopt,opterr)"
$ dbg="/NODEB/OPT=NOINLINE"
$ dbg2=dbg
$ if vax.and.decc then dbg2="/NODEB/NOOPT" !Kill VAX/DECC compiler bug
$!!old_cflags = "/DEFINE=(''cdefs'""hw_model=''f$getsyi(""hw_model"")'"")''dbg'"
$ cflags = "''dbg'''sp_cflgs'"
$ cflags2 = "''dbg2'''sp_cflgs'"
$ if "''cdefs'".nes."" then cflags = "/DEFINE=(''cdefs')''cflags'"
$ if "''cdefs'".nes."" then cflags2 = "/DEFINE=(''cdefs')''cflags2'"
$ if jump then goto rsacomp
$! set verify
$! library /create rsareflib
$!x='f$ver(0)'
$rsacomp:
$ if jump then if f$ele(0,"_",jumpto).nes."RSA" then goto pgpbuild
$ if jump then goto 'jumpto'
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!@pgpbuild
$pgpbuild:
$!cflags = "/DEFINE=(''cdefs'""hw_model=''f$getsyi(""hw_model"")'"")''dbg'"
$ cflags = "''dbg'''sp_cflgs'"
$ cflags2 = "''dbg2'''sp_cflgs'"
$ if "''cdefs'".nes."" then cflags = "/DEFINE=(''cdefs')''cflags'"
$ if "''cdefs'".nes."" then cflags2 = "/DEFINE=(''cdefs')''cflags2'"
$!
$! Now compile stuff
$!
$ if jump then if f$ele(0,"_",jumpto).nes."PGP" then goto pgplink
$ if jump then goto 'jumpto'
$ call pgp_compile pgp
$ if .not.f$int($status) then exit '$status
$ call pgp_compile config
$ if .not.f$int($status) then exit '$status
$ call pgp_compile crypto
$ if .not.f$int($status) then exit '$status
$ call pgp_compile keymgmt
$ if .not.f$int($status) then exit '$status
$ call pgp_compile keyadd
$ if .not.f$int($status) then exit '$status
$ call pgp_compile keymaint
$ if .not.f$int($status) then exit '$status
$ call pgp_compile fileio
$ if .not.f$int($status) then exit '$status
$ call pgp_compile mdfile
$ if .not.f$int($status) then exit '$status
$ call pgp_compile more
$ if .not.f$int($status) then exit '$status
$ call pgp_compile armor
$ if .not.f$int($status) then exit '$status
$ call pgp_compile mpilib
$ if .not.f$int($status) then exit '$status
$ call pgp_compile mpiio
$ if .not.f$int($status) then exit '$status
$ call pgp_compile getopt
$ if .not.f$int($status) then exit '$status
$ call pgp_compile genprime
$ if .not.f$int($status) then exit '$status
$ call pgp_compile rsagen
$ if .not.f$int($status) then exit '$status
$ call pgp_compile random
$ if .not.f$int($status) then exit '$status
$ call pgp_compile idea
$ if .not.f$int($status) then exit '$status
$ call pgp_compile passwd
$ if .not.f$int($status) then exit '$status
$ call pgp_compile md5 "''cflags2'"	!OVERRIDE /OPT STUFF FOR VAX/DECC!
$ if .not.f$int($status) then exit '$status
$ call pgp_compile system
$ if .not.f$int($status) then exit '$status
$ call pgp_compile language
$ if .not.f$int($status) then exit '$status
$ if vax
$ then
$   set ver
$   macro'macopts'/object='obj'vax.obj 'src'vax.mar
$!  x='f$ver(0)'
$ endif
$ if .not.f$int($status) then exit '$status
$ call pgp_compile charset
$ if .not.f$int($status) then exit '$status
$ call pgp_compile rsaglue1
$ if .not.f$int($status) then exit '$status
$ call pgp_compile noise
$ if .not.f$int($status) then exit '$status
$ call pgp_compile randpool
$ if .not.f$int($status) then exit '$status
$!Alter cc flags for z stuff
$ cflags = "/DEFINE=(''cdefs'EXPORT,NO_ASM,NOSTORE)''dbg'"
$ call pgp_compile zbits
$ if .not.f$int($status) then exit '$status
$ call pgp_compile zdeflate
$ if .not.f$int($status) then exit '$status
$ call pgp_compile zglobals
$ if .not.f$int($status) then exit '$status
$ call pgp_compile zinflate
$ if .not.f$int($status) then exit '$status
$ call pgp_compile zip
$ if .not.f$int($status) then exit '$status
$ call pgp_compile zipup
$ if .not.f$int($status) then exit '$status
$ call pgp_compile zfile_io
$ if .not.f$int($status) then exit '$status
$ call pgp_compile ztrees
$ if .not.f$int($status) then exit '$status
$ call pgp_compile zunzip
$ if .not.f$int($status) then exit '$status
$ jump=0
$ goto pgplink
$!Last Modified:  95.03.13 21.07.29.73
$ pgp_compile: subroutine	!p1 is file name
$ on control_c then goto bomb
$ on control_y then goto bomb
$ sfile = f$parse("''p1'","''src'","sys$disk:[].c")
$ ofile = f$parse("''obj'","''p1'","sys$disk:[].obj;")
$ define/user c$include 'src','rsa'
$ define/user vaxc$include sys$share:,'src','rsa'
$ define/user sys sys$share:
$ define/user decc$system_include sys$share:,'src','rsa'
$ __cflags = "''cflags'"
$ if "''p2'".nes."" then __cflags="''p2'"
$ set verify
$ 'cc' '__cflags' 'sfile'/object='ofile'
$!x='f$ver(0)'
$ exit 1
$bomb:
$!x='f$ver(0)'
$ say "%PGPINSTAL-F-INSABORT, Installation has been aborted"
$ exit 2160+%x10000000
$ endsubroutine
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!@pgplink
$pgplink:
$ if jump then if f$ele(0,"_",jumpto).nes."LNK" then goto badjump
$ if jump then goto 'jumpto'
$lnk_start: !jumpto entrypoint '$LNK_START'
$ open/write optfile pgp'pgpver'.opt
$ write optfile "NAME=PGP''pgpver'"
$ write optfile "IDENT=""PGP V''pgpver_human'"""
$ if vax then -
  write optfile "vax"
$ copy sys$input optfile
!
more
fileio
md5
mdfile
getopt
system
mpilib
mpiio
random
crypto
rsagen
noise
randpool
!
idea
passwd
genprime
pgp
config
language
charset
armor
!
! RSA Stuff
!
rsaglue1
!
! Key Stuff
!
keymgmt
keymaint
keyadd
!
! ZIP Stuff
!
zbits
zdeflate
zglobals
zinflate
zip
zipup
zfile_io
ztrees
zunzip
!
!rsareflib/library
$ eod
$ if vaxc then write optfile "sys$share:vaxcrtl/share"
$ close optfile
$lnk_link: !jumpto entrypoint '$LNK_LINK'
$ say "LINK /exe=PGP.EXE pgp''pgpver'.opt/opt''lnkopts'"
$ LINK /exe=PGP.EXE pgp'pgpver'.opt/opt'lnkopts'
$ say ""
$ pgpexe = f$search("pgp.exe")
$ if pgpexe.eqs.""
$ then
$   say "It appears that PGP.EXE was not built successfully"
$   say ""
$   say "Sorry."
$ else
$   say "PGP.EXE appears to have built successfully"
$   say ""
$   say "The executable is ''pgpexe'"
$   say ""
$   say "Set default to the pgp build directory and read the documents"
$   say "in the [.DOC] subdirectory regarding setting up PGP and creating"
$ exit
