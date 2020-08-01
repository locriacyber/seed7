# Makefile for gmake and gcc from MinGW. Commands executed by: cmd.exe
# To compile use a windows console and call:
#   make -f mk_djgpp.mak depend
#   make -f mk_djgpp.mak
# If your version of gmake does not support dos commands use MSYS with mk_msys.mak or mk_nmake.mak.

# CFLAGS = -O2 -fomit-frame-pointer -funroll-loops -Wall
# CFLAGS = -O2 -fomit-frame-pointer -Wall -Wstrict-prototypes -Winline -Wconversion -Wshadow -Wpointer-arith
CFLAGS = -O2 -g -Wall -Wstrict-prototypes -Winline -Wconversion -Wshadow -Wpointer-arith
# CFLAGS = -O2 -g -pg -Wall -Wstrict-prototypes -Winline -Wconversion -Wshadow -Wpointer-arith
# CFLAGS = -O2 -Wall -Wstrict-prototypes -Winline -Wconversion -Wshadow -Wpointer-arith
# CFLAGS = -O2 -pg -Wall -Wstrict-prototypes -Winline -Wconversion -Wshadow -Wpointer-arith
# CFLAGS = -O2 -funroll-loops -Wall -pg
# Since there is no linker option to determine the stack size
# it is determined with STACK_SIZE_DEFINITION (see below).
LDFLAGS =
# LDFLAGS = -pg
# LDFLAGS = -pg -lc_p
SYSTEM_LIBS = -lm
# SYSTEM_LIBS = -lm -lgmp
SYSTEM_DRAW_LIBS =
SEED7_LIB = seed7_05.a
DRAW_LIB = s7_draw.a
COMP_DATA_LIB = s7_data.a
COMPILER_LIB = s7_comp.a
ALL_S7_LIBS = ..\bin\$(COMPILER_LIB) ..\bin\$(COMP_DATA_LIB) ..\bin\$(DRAW_LIB) ..\bin\$(SEED7_LIB)
CC = gcc
GET_CC_VERSION_INFO = $(CC) --version >

BIGINT_LIB_DEFINE = USE_BIG_RTL_LIBRARY
BIGINT_LIB = big_rtl
# BIGINT_LIB_DEFINE = USE_BIG_GMP_LIBRARY
# BIGINT_LIB = big_gmp

# TERMINFO_OR_TERMCAP = USE_TERMINFO
# CONSOLE_OBJ = con_inf.o kbd_inf.o trm_inf.o
# CONSOLE_SRC = con_inf.c kbd_inf.c trm_inf.c
# TERMINFO_OR_TERMCAP = USE_TERMCAP
# CONSOLE_OBJ = con_inf.o kbd_inf.o trm_cap.o
# CONSOLE_SRC = con_inf.c kbd_inf.c trm_cap.c
# TERMINFO_OR_TERMCAP = USE_TERMINFO
# CONSOLE_OBJ = con_inf.o kbd_poll.o trm_inf.o
# CONSOLE_SRC = con_inf.c kbd_poll.c trm_inf.c
# TERMINFO_OR_TERMCAP = USE_TERMCAP
# CONSOLE_OBJ = con_inf.o kbd_poll.o trm_cap.o
# CONSOLE_SRC = con_inf.c kbd_poll.c trm_cap.c

# CONSOLE_OBJ = con_cur.o
# CONSOLE_SRC = con_cur.c
# CONSOLE_OBJ = con_cap.o
# CONSOLE_SRC = con_cap.c
# CONSOLE_OBJ = con_tcp.o
# CONSOLE_SRC = con_tcp.c
# CONSOLE_OBJ = con_x11.o
# CONSOLE_SRC = con_x11.c
CONSOLE_OBJ = con_wat.o
CONSOLE_SRC = con_wat.c

MOBJ1 = hi.o
POBJ1 = runerr.o option.o primitiv.o
LOBJ1 = actlib.o arrlib.o biglib.o blnlib.o bstlib.o chrlib.o cmdlib.o conlib.o dcllib.o drwlib.o
LOBJ2 = enulib.o fillib.o fltlib.o hshlib.o intlib.o itflib.o kbdlib.o lstlib.o prclib.o prglib.o
LOBJ3 = reflib.o rfllib.o sctlib.o setlib.o soclib.o strlib.o timlib.o typlib.o ut8lib.o
EOBJ1 = exec.o doany.o objutl.o
AOBJ1 = act_comp.o prg_comp.o analyze.o syntax.o token.o parser.o name.o type.o
AOBJ2 = expr.o atom.o object.o scanner.o literal.o numlit.o findid.o
AOBJ3 = error.o infile.o symbol.o info.o stat.o fatal.o match.o
GOBJ1 = syvarutl.o traceutl.o actutl.o executl.o blockutl.o
GOBJ2 = entutl.o identutl.o chclsutl.o sigutl.o
ROBJ1 = arr_rtl.o bln_rtl.o bst_rtl.o chr_rtl.o cmd_rtl.o con_rtl.o dir_rtl.o drw_rtl.o fil_rtl.o
ROBJ2 = flt_rtl.o hsh_rtl.o int_rtl.o kbd_rtl.o set_rtl.o soc_dos.o str_rtl.o tim_rtl.o ut8_rtl.o
ROBJ3 = heaputl.o striutl.o
DOBJ1 = $(BIGINT_LIB).o $(CONSOLE_OBJ) cmd_unx.o fil_dos.o tim_dos.o
OBJ = $(MOBJ1)
SEED7_LIB_OBJ = $(ROBJ1) $(ROBJ2) $(ROBJ3) $(DOBJ1)
DRAW_LIB_OBJ = drw_dos.o
COMP_DATA_LIB_OBJ = typ_data.o rfl_data.o ref_data.o listutl.o flistutl.o typeutl.o datautl.o
COMPILER_LIB_OBJ = $(POBJ1) $(LOBJ1) $(LOBJ2) $(LOBJ3) $(EOBJ1) $(AOBJ1) $(AOBJ2) $(AOBJ3) $(GOBJ1) $(GOBJ2)

MSRC1 = hi.c
PSRC1 = runerr.c option.c primitiv.c
LSRC1 = actlib.c arrlib.c biglib.c blnlib.c bstlib.c chrlib.c cmdlib.c conlib.c dcllib.c drwlib.c
LSRC2 = enulib.c fillib.c fltlib.c hshlib.c intlib.c itflib.c kbdlib.c lstlib.c prclib.c prglib.c
LSRC3 = reflib.c rfllib.c sctlib.c setlib.c soclib.c strlib.c timlib.c typlib.c ut8lib.c
ESRC1 = exec.c doany.c objutl.c
ASRC1 = act_comp.c prg_comp.c analyze.c syntax.c token.c parser.c name.c type.c
ASRC2 = expr.c atom.c object.c scanner.c literal.c numlit.c findid.c
ASRC3 = error.c infile.c symbol.c info.c stat.c fatal.c match.c
GSRC1 = syvarutl.c traceutl.c actutl.c executl.c blockutl.c
GSRC2 = entutl.c identutl.c chclsutl.c sigutl.c
RSRC1 = arr_rtl.c bln_rtl.c bst_rtl.c chr_rtl.c cmd_rtl.c con_rtl.c dir_rtl.c drw_rtl.c fil_rtl.c
RSRC2 = flt_rtl.c hsh_rtl.c int_rtl.c kbd_rtl.c set_rtl.c soc_dos.c str_rtl.c tim_rtl.c ut8_rtl.c
RSRC3 = heaputl.c striutl.c
DSRC1 = $(BIGINT_LIB).c $(CONSOLE_SRC) cmd_unx.c fil_dos.c tim_dos.c
SRC = $(MSRC1)
SEED7_LIB_SRC = $(RSRC1) $(RSRC2) $(RSRC3) $(DSRC1)
DRAW_LIB_SRC = drw_dos.c
COMP_DATA_LIB_SRC = typ_data.c rfl_data.c ref_data.c listutl.c flistutl.c typeutl.c datautl.c
COMPILER_LIB_SRC = $(PSRC1) $(LSRC1) $(LSRC2) $(LSRC3) $(ESRC1) $(ASRC1) $(ASRC2) $(ASRC3) $(GSRC1) $(GSRC2)

..\bin\hi.exe: $(OBJ) $(ALL_S7_LIBS)
	$(CC) $(LDFLAGS) $(OBJ) $(ALL_S7_LIBS) $(SYSTEM_DRAW_LIBS) $(SYSTEM_LIBS) -o ..\bin\hi
	copy ..\bin\hi.exe ..\prg
	..\bin\hi level

hi: ..\bin\hi.exe

clear: clean

clean:
	del version.h
	del *.o
	del ..\bin\*.a
	del depend

dep: depend

strip:
	strip ..\bin\hi.exe

version.h:
	echo #define ANSI_C > version.h
	echo #define USE_DIRENT >> version.h
	echo #define PATH_DELIMITER 92 /* backslash (ASCII) */ >> version.h
	echo #define SEARCH_PATH_DELIMITER ';' >> version.h
	echo #define MAP_ABSOLUTE_PATH_TO_DRIVE_LETTERS >> version.h
	echo #define CATCH_SIGNALS >> version.h
	echo #define AWAIT_WITH_SELECT >> version.h
	echo #define OS_STRI_USES_CODEPAGE >> version.h
	echo #define os_lstat stat >> version.h
	echo #define os_fseek fseek >> version.h
	echo #define os_ftell ftell >> version.h
	echo #define OS_FSEEK_OFFSET_BITS 32 >> version.h
	echo #define os_off_t off_t >> version.h
	echo #define os_putenv putenv >> version.h
	echo #define $(BIGINT_LIB_DEFINE) >> version.h
	echo #define likely(x)   __builtin_expect((x),1) >> version.h
	echo #define unlikely(x) __builtin_expect((x),0) >> version.h
	$(GET_CC_VERSION_INFO) cc_vers.txt
	echo #include "direct.h" > chkccomp.h
	echo #define mkdir(path,mode) mkdir(path) >> chkccomp.h
	echo #define LIST_DIRECTORY_CONTENTS "dir" >> chkccomp.h
	echo #define long_long_EXISTS >> chkccomp.h
	echo #define long_long_SUFFIX_LL >> chkccomp.h
	$(CC) chkccomp.c -lm -o chkccomp
	.\chkccomp.exe >> version.h
	del chkccomp.h
	del chkccomp.exe
	del cc_vers.txt
	echo #define OBJECT_FILE_EXTENSION ".o" >> version.h
	echo #define LIBRARY_FILE_EXTENSION ".a" >> version.h
	echo #define EXECUTABLE_FILE_EXTENSION ".exe" >> version.h
	echo #define C_COMPILER "$(CC)" >> version.h
	echo #define GET_CC_VERSION_INFO "$(GET_CC_VERSION_INFO)" >> version.h
	echo #define CC_OPT_DEBUG_INFO "-g" >> version.h
	echo #define CC_OPT_NO_WARNINGS "-w" >> version.h
	echo #define LINKER_OPT_OUTPUT_FILE "-o " >> version.h
	echo #define LINKER_FLAGS "$(LDFLAGS)" >> version.h
	echo #define SYSTEM_LIBS "$(SYSTEM_LIBS)" >> version.h
	echo #define SYSTEM_DRAW_LIBS "$(SYSTEM_DRAW_LIBS)" >> version.h
	echo #define SEED7_LIB "$(SEED7_LIB)" >> version.h
	echo #define DRAW_LIB "$(DRAW_LIB)" >> version.h
	echo #define COMP_DATA_LIB "$(COMP_DATA_LIB)" >> version.h
	echo #define COMPILER_LIB "$(COMPILER_LIB)" >> version.h
	echo #define STACK_SIZE_DEFINITION unsigned _stklen = 4194304 >> version.h
	$(CC) -o setpaths setpaths.c
	.\setpaths.exe >> version.h
	del setpaths.exe

depend: version.h
	echo Working without C header dependency checks.

level.h:
	..\bin\hi level

..\bin\$(SEED7_LIB): $(SEED7_LIB_OBJ)
	ar r ..\bin\$(SEED7_LIB) $(SEED7_LIB_OBJ)

..\bin\$(DRAW_LIB): $(DRAW_LIB_OBJ)
	ar r ..\bin\$(DRAW_LIB) $(DRAW_LIB_OBJ)

..\bin\$(COMP_DATA_LIB): $(COMP_DATA_LIB_OBJ)
	ar r ..\bin\$(COMP_DATA_LIB) $(COMP_DATA_LIB_OBJ)

..\bin\$(COMPILER_LIB): $(COMPILER_LIB_OBJ)
	ar r ..\bin\$(COMPILER_LIB) $(COMPILER_LIB_OBJ)

wc: $(SRC)
	echo SRC:
	wc $(SRC)
	echo SEED7_LIB_SRC:
	wc $(SEED7_LIB_SRC)
	echo DRAW_LIB_SRC:
	wc $(DRAW_LIB_SRC)
	echo COMP_DATA_LIB_SRC:
	wc $(COMP_DATA_LIB_SRC)
	echo COMPILER_LIB_SRC:
	wc $(COMPILER_LIB_SRC)

lint: $(SRC)
	lint -p $(SRC) $(SYSTEM_DRAW_LIBS) $(SYSTEM_LIBS)

lint2: $(SRC)
	lint -Zn2048 $(SRC) $(SYSTEM_DRAW_LIBS) $(SYSTEM_LIBS)
