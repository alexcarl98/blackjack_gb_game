@REM things passed into the linker are -Wl, things passed to frontend are -Wf
mkdir obj
c:\gbdk\bin\lcc     -c -o obj\common.o source\common.c
c:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wl-y -Wl-w -c -o obj\main.o source\main.c
c:\gbdk\bin\lcc     -c -o obj\bkg_single_cards.o res\bkg_single_cards.c
c:\gbdk\bin\lcc     -c -o obj\bkg_sc_c1.o res\bkg_sc_c1.c
c:\gbdk\bin\lcc     -c -o obj\bkg_hs_opt.o res\bkg_hs_opt.c
c:\gbdk\bin\lcc     -c -o obj\bkg_win.o res\bkg_win.c
c:\gbdk\bin\lcc     -c -o obj\bkg_pushgm.o res\bkg_pushgm.c
c:\gbdk\bin\lcc     -c -o obj\bkg_game_info.o res\bkg_game_info.c

c:\gbdk\bin\lcc     -c -o obj\bkg_shuf.o res\bkg_shuf.c
c:\gbdk\bin\lcc     -c -o obj\hole_card.o res\hole_card.c

c:\gbdk\bin\lcc     -c -o obj\bkg_lose.o res\bkg_lose.c
c:\gbdk\bin\lcc     -c -o obj\bkg_sur.o res\bkg_sur.c
c:\gbdk\bin\lcc     -c -o obj\bkg_table.o res\bkg_table.c
c:\gbdk\bin\lcc     -c -o obj\bkg_tiles_alt.o res\bkg_tiles_alt.c
c:\gbdk\bin\lcc     -c -o obj\bkg_insurance.o res\bkg_insurance.c
c:\gbdk\bin\lcc     -c -o obj\card_sprites.o res\card_sprites.c
c:\gbdk\bin\lcc     -c -o obj\bkg_win_cash_bet.o res\bkg_win_cash_bet.c

c:\gbdk\bin\lcc     -c -o obj\change_bet.o res\change_bet.c
c:\gbdk\bin\lcc     -c -o obj\chips_win.o res\chips_win.c
c:\gbdk\bin\lcc     -c -o obj\hole_card.o res\hole_card.c
c:\gbdk\bin\lcc     -c -o obj\table_change.o res\table_change.c

c:\gbdk\bin\lcc     -c -o obj\starting_amt.o \res\starting_amt.c
c:\gbdk\bin\lcc     -c -o obj\end_amt.o \res\end_amt.c
c:\gbdk\bin\lcc     -c -o obj\press_to_chg_bet.o \res\press_to_chg_bet.c

c:\gbdk\bin\lcc     -c -o obj\table_change.o res\bkg_tiles_o.c

@REM c:\gbdk\bin\lcc     -c -o obj\splash.o res_\splash.c
@REM c:\gbdk\bin\lcc     -c -o obj\king.o res_\king.c

c:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wl-y -Wl-w -o obj\main.gb obj\*.o
@REM Compile a .gb file from the compiled .o files
@REM obj\bkg_single_cards.o


@REM @REM (w/saves):
@REM c:\gbdk\bin\lcc -Wa-l -Wf-ba0 -c -o savestate.o savestate.c
@REM c:\gbdk\bin\lcc -Wa-l -c -o misc_tester.o misc_tester.c

@REM c:\gbdk\bin\lcc -Wl-yt3 -Wl-yo4 -Wl-ya4 -o misc_tester.gb misc_tester.o savestate.o

@REM delete intermediate files created for the compilation process
@REM DEL *.asm
@REM DEL *.lst
@REM DEL *.ihx
@REM DEL *.sym
@REM DEL *.o
@REM DEL *.map
@REM DEL *.adb