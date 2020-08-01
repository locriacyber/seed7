# Makefile for nmake from windows and msvc.
# To compile use a windows console and call:
#   nmake /f mk_msvc.mak depend
#   nmake /f mk_msvc.mak
# If you use MinGW you should use mk_mingw.mak, mk_nmake.mak or mk_msys.mak instead.

# CFLAGS = -AL -Ozax -Gr -Gs -Gm -G0 -W4
CFLAGS = -O2 -W4
# CFLAGS = /O2 -Wall -Wstrict-prototypes -Winline -Wconversion -Wshadow -Wpointer-arith
# CFLAGS = /O2 -pg -Wall -Wstrict-prototypes -Winline -Wconversion -Wshadow -Wpointer-arith
# CFLAGS = /O2 -funroll-loops -Wall -pg
LFLAGS = -O2
# LFLAGS = -O2 -pg
LIBS = user32.lib gdi32.lib
CC = cl

# SCREEN_OBJ = scr_x11.obj
# SCREEN_SRC = scr_x11.c
# SCREEN_OBJ = scr_infi.obj kbd_infi.obj trm_inf.obj
# SCREEN_SRC = scr_inf.c kbd_inf.c trm_inf.c
# SCREEN_OBJ = scr_infp.obj kbd_infp.obj trm_cap.obj
# SCREEN_SRC = scr_inf.c kbd_inf.c trm_cap.c
# SCREEN_OBJ = scr_cur.obj
# SCREEN_SRC = scr_cur.c
# SCREEN_OBJ = scr_cap.obj
# SCREEN_SRC = scr_cap.c
# SCREEN_OBJ = scr_tcp.obj
# SCREEN_SRC = scr_tcp.c
SCREEN_OBJ = scr_win.obj
SCREEN_SRC = scr_win.c

MOBJ1 = hi.obj option.obj primitiv.obj
LOBJ1 = actlib.obj arrlib.obj biglib.obj blnlib.obj chrlib.obj clslib.obj cmdlib.obj dcllib.obj drwlib.obj
LOBJ2 = enulib.obj fillib.obj fltlib.obj hshlib.obj intlib.obj kbdlib.obj lstlib.obj prclib.obj prglib.obj
LOBJ3 = reflib.obj rfllib.obj scrlib.obj sctlib.obj setlib.obj strlib.obj timlib.obj typlib.obj ut8lib.obj
EOBJ1 = exec.obj doany.obj memory.obj runerr.obj
AOBJ1 = analyze.obj syntax.obj token.obj parser.obj name.obj type.obj
AOBJ2 = expr.obj atom.obj object.obj scanner.obj literal.obj numlit.obj findid.obj
AOBJ3 = error.obj infile.obj symbol.obj info.obj stat.obj fatal.obj match.obj
GOBJ1 = syvarutl.obj traceutl.obj actutl.obj listutl.obj arrutl.obj executl.obj blockutl.obj
GOBJ2 = typeutl.obj entutl.obj identutl.obj chclsutl.obj flistutl.obj sigutl.obj
ROBJ1 = arr_rtl.obj big_rtl.obj bln_rtl.obj chr_rtl.obj cmd_rtl.obj fil_rtl.obj flt_rtl.obj hsh_rtl.obj
ROBJ2 = int_rtl.obj kbd_rtl.obj scr_rtl.obj set_rtl.obj str_rtl.obj ut8_rtl.obj heaputl.obj striutl.obj
DOBJ1 = $(SCREEN_OBJ) tim_win.obj drw_win.obj dir_win.obj
OBJ = $(MOBJ1) $(LOBJ1) $(LOBJ2) $(LOBJ3) $(EOBJ1) $(AOBJ1) $(AOBJ2) $(AOBJ3) $(GOBJ1) $(GOBJ2)
A_OBJ = $(ROBJ1) $(ROBJ2) $(DOBJ1)

MSRC1 = hi.c option.c primitiv.c
LSRC1 = actlib.c arrlib.c biglib.c blnlib.c chrlib.c clslib.c cmdlib.c dcllib.c drwlib.c
LSRC2 = enulib.c fillib.c fltlib.c hshlib.c intlib.c kbdlib.c lstlib.c prclib.c prglib.c
LSRC3 = reflib.c rfllib.c scrlib.c sctlib.c setlib.c strlib.c timlib.c typlib.c ut8lib.c
ESRC1 = exec.c doany.c memory.c runerr.c
ASRC1 = analyze.c syntax.c token.c parser.c name.c type.c
ASRC2 = expr.c atom.c object.c scanner.c literal.c numlit.c findid.c
ASRC3 = error.c infile.c symbol.c info.c stat.c fatal.c match.c
GSRC1 = syvarutl.c traceutl.c actutl.c listutl.c arrutl.c executl.c blockutl.c
GSRC2 = typeutl.c entutl.c identutl.c chclsutl.c flistutl.c sigutl.c
RSRC1 = arr_rtl.c big_rtl.c bln_rtl.c chr_rtl.c cmd_rtl.c fil_rtl.c flt_rtl.c hsh_rtl.c
RSRC2 = int_rtl.c kbd_rtl.c scr_rtl.c set_rtl.c str_rtl.c ut8_rtl.c heaputl.c striutl.c
DSRC1 = $(SCREEN_SRC) tim_win.c drw_win.c
SRC = $(MSRC1) $(LSRC1) $(LSRC2) $(LSRC3) $(ESRC1) $(ASRC1) $(ASRC2) $(ASRC3) $(GSRC1) $(GSRC2)
A_SRC = $(RSRC1) $(RSRC2) $(DSRC1)

hi: $(OBJ) seed7_05.lib
	$(CC) $(LFLAGS) $(OBJ) seed7_05.lib $(LIBS) -o hi
	copy hi.exe ..\prg /Y
	.\hi level

hi.gp: $(OBJ)
	$(CC) $(LFLAGS) $(OBJ) $(LIBS) -o /usr/local/bin/hi.gp
	hi level

scr_x11.obj: scr_x11.c version.h scr_drv.h trm_drv.h
	$(CC) $(CFLAGS) -c scr_x11.c

scr_infi.obj: scr_inf.c version.h scr_drv.h trm_drv.h
	echo "#undef  USE_TERMCAP" > inf_conf.h
	$(CC) $(CFLAGS) -c scr_inf.c
	mv scr_inf.obj scr_infi.obj

scr_infp.obj: scr_inf.c version.h scr_drv.h trm_drv.h
	echo "#define USE_TERMCAP" > inf_conf.h
	$(CC) $(CFLAGS) -c scr_inf.c
	mv scr_inf.obj scr_infp.obj

kbd_infi.obj: kbd_inf.c version.h kbd_drv.h trm_drv.h
	echo "#undef  USE_TERMCAP" > inf_conf.h
	$(CC) $(CFLAGS) -c kbd_inf.c
	mv kbd_inf.obj kbd_infi.obj

kbd_infp.obj: kbd_inf.c version.h kbd_drv.h trm_drv.h
	echo "#define USE_TERMCAP" > inf_conf.h
	$(CC) $(CFLAGS) -c kbd_inf.c
	mv kbd_inf.obj kbd_infp.obj

trm_inf.obj: trm_inf.c version.h trm_drv.h
	$(CC) $(CFLAGS) -c trm_inf.c

trm_cap.obj: trm_cap.c version.h trm_drv.h
	$(CC) $(CFLAGS) -c trm_cap.c

scr_cur.obj: scr_cur.c version.h scr_drv.h
	$(CC) $(CFLAGS) -c scr_cur.c


clear:
	del *.obj
	del *.lib
	del depend
	del a_depend
	del version.h

dep: depend

strip:
	strip /usr/local/bin/hi

version.h:
	echo #define ANSI_C > version.h
	echo #define USE_DIRWIN >> version.h
	echo #define PATH_DELIMITER '/' >> version.h
	echo #define CATCH_SIGNALS >> version.h
	echo #undef  USE_MMAP >> version.h
	echo #undef  INCL_NCURSES_TERM >> version.h
	echo #undef  INCL_CURSES_BEFORE_TERM >> version.h
	echo #define MKDIR_WITH_ONE_PARAMETER >> version.h
	echo #define CHOWN_MISSING >> version.h
	echo #undef  CHMOD_MISSING >> version.h
	echo #define DEFINE_IEEE_754_STUFF >> version.h
	echo #define FLOAT_ZERO_DIV_ERROR >> version.h
	echo #define USE_MYUNISTD_H >> version.h
	echo #undef  USE_FSEEKO64 >> version.h
	echo #define LINKER_LIBS "$(LIBS)" >> version.h
	echo #include "stdio.h" > libpath.c
	echo #include "stddef.h" >> libpath.c
	echo int chdir(char *path); >> libpath.c
	echo char *getcwd(char *buf, size_t size); >> libpath.c
	echo int main (int argc, char **argv) >> libpath.c
	echo { >> libpath.c
	echo char buffer[4096]; >> libpath.c
	echo int position; >> libpath.c
	echo chdir("../lib"); >> libpath.c
	echo getcwd(buffer, sizeof(buffer)); >> libpath.c
	echo printf("\043define SEED7_LIBRARY \042"); >> libpath.c
	echo for (position = 0; buffer[position] != '\0'; position++) { >> libpath.c
	echo putchar(buffer[position] == '\\' ? '/' : buffer[position]); >> libpath.c
	echo } >> libpath.c
	echo printf("\042\n"); >> libpath.c
	echo return 0; >> libpath.c
	echo } >> libpath.c
	$(CC) libpath.c -o libpath
	libpath >> ..\src\version.h
	del libpath.c
	del libpath.obj
	del libpath.exe

hi.obj: hi.c
	$(CC) $(CFLAGS) -c hi.c

.c.o:
	$(CC) $(CFLAGS) -c $<

depend: version.h
	echo Working without dependency file

level.h:
	hi level

seed7_05.lib: $(A_OBJ)
	lib /out:seed7_05.lib $(A_OBJ)

wc: $(SRC)
	wc $(GSRC1) $(GSRC2)
	wc $(ASRC1) $(ASRC2) $(ASRC3)
	wc $(RSRC1) $(RSRC2)
	wc $(ESRC1)
	wc $(LSRC1) $(LSRC2) $(LSRC3)
	wc $(DSRC1)
	wc $(MSRC1)
	wc $(SRC)

lint: $(SRC)
	lint -p $(SRC) $(LIBS)

lint2: $(SRC)
	lint -Zn2048 $(SRC) $(LIBS)
