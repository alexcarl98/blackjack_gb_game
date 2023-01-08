@REM things passed into the linker are -Wl, things passed to frontend are -Wf
c:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wl-y -Wl-w -c -o main.o main.c

@REM Compile a .gb file from the compiled .o files
c:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wl-y -Wl-w -o main.gb main.o


@REM @REM (w/saves):
@REM c:\gbdk\bin\lcc -Wa-l -Wf-ba0 -c -o savestate.o savestate.c
@REM c:\gbdk\bin\lcc -Wa-l -c -o misc_tester.o misc_tester.c

@REM c:\gbdk\bin\lcc -Wl-yt3 -Wl-yo4 -Wl-ya4 -o misc_tester.gb misc_tester.o savestate.o

@REM delete intermediate files created for the compilation process
DEL *.asm
DEL *.lst
DEL *.ihx
DEL *.sym
DEL *.o
