@echo off 
@rem Convert PostScript to PDF.

if "%1"=="" goto usage
if "%2"=="" goto usage
if not "%3"=="" goto usage

rem Watcom C deletes = signs, so use # instead.
gs -q -dNOPAUSE -sDEVICE#pdfwrite -sOutputFile#%2 %1 -c quit
goto end

:usage
echo "Usage: ps2pdf input.ps output.pdf"

:end
