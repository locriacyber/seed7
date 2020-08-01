/********************************************************************/
/*                                                                  */
/*  scr_rtl.c     Primitive actions for screen output.              */
/*  Copyright (C) 1989 - 2007  Thomas Mertes                        */
/*                                                                  */
/*  This file is part of the Seed7 Runtime Library.                 */
/*                                                                  */
/*  The Seed7 Runtime Library is free software; you can             */
/*  redistribute it and/or modify it under the terms of the GNU     */
/*  Lesser General Public License as published by the Free Software */
/*  Foundation; either version 2.1 of the License, or (at your      */
/*  option) any later version.                                      */
/*                                                                  */
/*  The Seed7 Runtime Library is distributed in the hope that it    */
/*  will be useful, but WITHOUT ANY WARRANTY; without even the      */
/*  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR */
/*  PURPOSE.  See the GNU Lesser General Public License for more    */
/*  details.                                                        */
/*                                                                  */
/*  You should have received a copy of the GNU Lesser General       */
/*  Public License along with this program; if not, write to the    */
/*  Free Software Foundation, Inc., 59 Temple Place, Suite 330,     */
/*  Boston, MA 02111-1307 USA                                       */
/*                                                                  */
/*  Module: Seed7 Runtime Library                                   */
/*  File: seed7/src/scr_rtl.c                                       */
/*  Changes: 2007  Thomas Mertes                                    */
/*  Content: Primitive actions for screen output.                   */
/*                                                                  */
/********************************************************************/

#include "version.h"

#include "stdio.h"

#include "common.h"
#include "striutl.h"
#include "scr_drv.h"

#undef EXTERN
#define EXTERN
#include "scr_rtl.h"


static inttype cursor_line = 1;
static inttype cursor_column = 1;



#ifdef ANSI_C

void scrHScroll (inttype startlin, inttype startcol,
    inttype stoplin, inttype stopcol, inttype count)
#else

void scrHScroll (startlin, startcol, stoplin, stopcol, count)
inttype startlin;
inttype startcol;
inttype stoplin;
inttype stopcol;
inttype count;
#endif

  { /* scrHScroll */
    if (count >= 0) {
      scrLeftScroll(startlin, startcol, stoplin, stopcol, count);
    } else {
      scrRightScroll(startlin, startcol, stoplin, stopcol, -count);
    } /* if */
  } /* scrHScroll */



#ifdef ANSI_C

void scrSetpos (inttype lin, inttype col)
#else

void scrSetpos (lin, col)
inttype lin;
inttype col;
#endif

  { /* scrSetpos */
    cursor_line = lin;
    cursor_column = col;
    scrSetCursor(lin, col);
  } /* scrSetpos */



#ifdef ANSI_C

void scrVScroll (inttype startlin, inttype startcol,
    inttype stoplin, inttype stopcol, inttype count)
#else

void scrVScroll (startlin, startcol, stoplin, stopcol, count)
inttype startlin;
inttype startcol;
inttype stoplin;
inttype stopcol;
inttype count;
#endif

  { /* scrVScroll */
    if (count >= 0) {
      scrUpScroll(startlin, startcol, stoplin, stopcol, count);
    } else {
      scrDownScroll(startlin, startcol, stoplin, stopcol, -count);
    } /* if */
  } /* scrVScroll */



#ifdef ANSI_C

void scrWrite (stritype stri)
#else

void scrWrite (stri)
stritype stri;
#endif

  /* This function writes the string stri to the screen at the      */
  /* current position. The current position must be a legal         */
  /* position of the screen. The string stri is not allowed to go   */
  /* beyond the right border of the screen. All screen output       */
  /* must be done with this function.                               */

  { /* scrWrite */
#ifdef WIDE_CHAR_STRINGS
    {
      uchartype stri_buffer[2000];

      stri_compress(stri_buffer, stri->mem, stri->size);
      scrText(cursor_line, cursor_column,
        stri_buffer, stri->size);
    }
#else
    scrText(cursor_line, cursor_column,
        stri->mem, stri->size);
#endif
    cursor_column = cursor_column + stri->size;
  } /* scrWrite */
