/********************************************************************/
/*                                                                  */
/*  str_rtl.c     Primitive actions for the string type.            */
/*  Copyright (C) 1989 - 2015  Thomas Mertes                        */
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
/*  Free Software Foundation, Inc., 51 Franklin Street,             */
/*  Fifth Floor, Boston, MA  02110-1301, USA.                       */
/*                                                                  */
/*  Module: Seed7 Runtime Library                                   */
/*  File: seed7/src/str_rtl.c                                       */
/*  Changes: 1991 - 1994, 2005, 2008 - 2015  Thomas Mertes          */
/*  Content: Primitive actions for the string type.                 */
/*                                                                  */
/********************************************************************/

#include "version.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "common.h"
#include "data_rtl.h"
#include "heaputl.h"
#include "striutl.h"
#include "rtl_err.h"

#undef EXTERN
#define EXTERN
#include "str_rtl.h"

#undef TRACE_STR_RTL


#define CHAR_DELTA_BEYOND 128

/* memset_to_strelem is not used because it is */
/* only better for lengths greater than 7.     */
#define LPAD_WITH_MEMSET_TO_STRELEM 0



static inline int strelem_memcmp (const strElemType *mem1,
    const strElemType *mem2, size_t number)

  { /* strelem_memcmp */
    for (; number > 0; mem1++, mem2++, number--) {
      if (*mem1 != *mem2) {
        return *mem1 < *mem2 ? -1 : 1;
      } /* if */
    } /* for */
    return 0;
  } /* strelem_memcmp */



static inline const strElemType *search_strelem (const strElemType *mem,
    const strElemType ch, const strElemType *const beyond)

  { /* search_strelem */
    for (; mem != beyond; mem++) {
      if (*mem == ch) {
        return mem;
      } /* if */
    } /* for */
    return NULL;
  } /* search_strelem */



static inline const strElemType *search_strelem2 (const strElemType *mem,
    const strElemType ch, const strElemType *const beyond,
    const memSizeType charDelta[])

  { /* search_strelem2 */
    while (mem < beyond) {
      if (*mem == ch) {
        return mem;
      } else if (*mem < CHAR_DELTA_BEYOND) {
        mem += charDelta[*mem];
      } else {
        mem += charDelta[CHAR_DELTA_BEYOND];
      } /* if */
    } /* while */
    return NULL;
  } /* search_strelem2 */



static inline const strElemType *rsearch_strelem (const strElemType *mem,
    const strElemType ch, size_t number)

  { /* rsearch_strelem */
    for (; number > 0; mem--, number--) {
      if (*mem == ch) {
        return mem;
      } /* if */
    } /* for */
    return NULL;
  } /* rsearch_strelem */



static inline const strElemType *rsearch_strelem2 (const strElemType *mem,
    const strElemType ch, const strElemType *const beyond,
    const memSizeType charDelta[])

  { /* rsearch_strelem2 */
    while (mem > beyond) {
      if (*mem == ch) {
        return mem;
      } else if (*mem < CHAR_DELTA_BEYOND) {
        mem -= charDelta[*mem];
      } else {
        mem -= charDelta[CHAR_DELTA_BEYOND];
      } /* if */
    } /* while */
    return NULL;
  } /* rsearch_strelem2 */



void toLower (const strElemType *const stri, memSizeType length,
    strElemType *const dest)

  {
    memSizeType pos;
    strElemType ch;

  /* toLower */
    for (pos = 0; pos < length; pos++) {
      ch = stri[pos];
      switch (ch >> 8) {
        case 0:
          if (ch <= '\177') {
            ch = (strElemType) ((unsigned char)
                "\0\1\2\3\4\5\6\7\10\11\12\13\14\15\16\17"
                "\20\21\22\23\24\25\26\27\30\31\32\33\34\35\36\37"
                " !\"#$%&'()*+,-./0123456789:;<=>?"
                "@abcdefghijklmnopqrstuvwxyz[\\]^_"
                "`abcdefghijklmnopqrstuvwxyz{|}~\177"[ch]);
          } else if ("\0\0\0\0\0\0\0\0\376\377\377\007\0\0\0\0"
              "\0\0\0\0\0\0\0\0\377\377\177\177\0\0\0\0"[ch >> 3 & 31] &
              1 << (ch & 7)) {
            ch += 32;
          } /* if */
          break;
        case 1:
          if ("UUUUUUU\252\252TUUUUU+"
              "\326\316\333\261\325\322\256\021\260\255\252JUU\326U"[ch >> 3 & 31] &
              1 << (ch & 7)) {
            switch (ch) {
              case 0x0130: ch = 0x0069; break;
              case 0x0178: ch = 0x00ff; break;
              case 0x0181: ch = 0x0253; break;
              case 0x0186: ch = 0x0254; break;
              case 0x0189: ch = 0x0256; break;
              case 0x018a: ch = 0x0257; break;
              case 0x018e: ch = 0x01dd; break;
              case 0x018f: ch = 0x0259; break;
              case 0x0190: ch = 0x025b; break;
              case 0x0193: ch = 0x0260; break;
              case 0x0194: ch = 0x0263; break;
              case 0x0196: ch = 0x0269; break;
              case 0x0197: ch = 0x0268; break;
              case 0x019c: ch = 0x026f; break;
              case 0x019d: ch = 0x0272; break;
              case 0x019f: ch = 0x0275; break;
              case 0x01a6: ch = 0x0280; break;
              case 0x01a9: ch = 0x0283; break;
              case 0x01ae: ch = 0x0288; break;
              case 0x01b1: ch = 0x028a; break;
              case 0x01b2: ch = 0x028b; break;
              case 0x01b7: ch = 0x0292; break;
              case 0x01c4: ch += 2;     break;
              case 0x01c7: ch += 2;     break;
              case 0x01ca: ch += 2;     break;
              case 0x01f1: ch += 2;     break;
              case 0x01f2: ch = 0x01f3; break;
              case 0x01f6: ch = 0x0195; break;
              case 0x01f7: ch = 0x01bf; break;
              case 0x01f8: ch = 0x01f9; break;
              default: ch += 1;         break;
            } /* switch */
          } /* if */
          break;
        case 2:
          if ("UUUUUU\005lzU\0\0\0\0\0\0"
              "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"[ch >> 3 & 31] &
              1 << (ch & 7)) {
            switch (ch) {
              case 0x0220: ch -= 130;   break;
              case 0x023a: ch = 0x2c65; break;
              case 0x023d: ch -= 163;   break;
              case 0x023e: ch = 0x2c66; break;
              case 0x0243: ch -= 195;   break;
              case 0x0244: ch += 69;    break;
              case 0x0245: ch += 71;    break;
              default:     ch += 1;     break;
            } /* switch */
          } /* if */
          break;
        case 3:
          if ("\0\0\0\0\0\0\0\0\0\0\0\0\0\0E\0"
              "@\327\376\377\373\017\0\0\0\200\0UUU\220\346"[ch >> 3 & 31] &
              1 << (ch & 7)) {
            switch (ch) {
              case 0x0370: ch += 1;   break;
              case 0x0372: ch += 1;   break;
              case 0x0376: ch += 1;   break;
              case 0x0386: ch += 38;  break;
              case 0x0388: ch += 37;  break;
              case 0x0389: ch += 37;  break;
              case 0x038a: ch += 37;  break;
              case 0x038c: ch += 64;  break;
              case 0x038e: ch += 63;  break;
              case 0x038f: ch += 63;  break;
              case 0x03cf: ch += 8;   break;
              case 0x03f4: ch -= 60;  break;
              case 0x03f9: ch -= 7;   break;
              case 0x03fd: ch -= 130; break;
              case 0x03fe: ch -= 130; break;
              case 0x03ff: ch -= 130; break;
              default:
                if (ch <= 0x03ab) {
                  ch += 32;
                } else {
                  ch += 1;
                } /* if */
                break;
            } /* switch */
          } /* if */
          break;
        case 4:
          if ("\377\377\377\377\377\377\0\0\0\0\0\0UUUU"
              "\001TUUUUUU\253*UUUUUU"[ch >> 3 & 31] &
              1 << (ch & 7)) {
            if (ch <= 0x040f) {
              ch += 80;
            } else if (ch <= 0x042f) {
              ch += 32;
            } else if (ch == 0x04c0) {
              ch = 0x04cf;
            } else {
              ch += 1;
            } /* if */
          } /* if */
          break;
        case 5:
          if (ch <= 0x0526 && (ch & 1) == 0) {
            ch += 1;
          } else if (ch >= 0x0531 && ch <= 0x0556) {
            ch += 48;
          } /* if */
          break;
        case 16:
          if ("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
              "\0\0\0\0\377\377\377\377\277 \0\0\0\0\0\0"[ch >> 3 & 31] &
              1 << (ch & 7)) {
            ch += 0x1c60;
          } /* if */
          break;
        case 30:
          if ("UUUUUUUUUUUUUUUUUU\025@UUUUUUUUUUUU"[ch >> 3 & 31] &
              1 << (ch & 7)) {
            if (ch == 0x1e9e) {
              ch = 0x00df;
            } else {
              ch += 1;
            } /* if */
          } /* if */
          break;
        case 31:
          if ("\0\377\0?\0\377\0\377\0?\0\252\0\377\0\0"
              "\0\377\0\377\0\377\0\037\0\037\0\017\0\037\0\037"[ch >> 3 & 31] &
              1 << (ch & 7)) {
            if (ch >= 0x1fba) {
              ch = (strElemType) ((unsigned char)
                  "\160\161\263\000\000\000\000\000\000\000"
                  "\000\000\000\000\162\163\164\165\303\000"
                  "\000\000\000\000\000\000\000\000\000\000"
                  "\320\321\166\167\000\000\000\000\000\000"
                  "\000\000\000\000\000\000\340\341\172\173"
                  "\345\000\000\000\000\000\000\000\000\000"
                  "\000\000\170\171\174\175\363"[ch - 0x1fba] + 0x1f00);
            } else {
              ch -= 8;
            } /* if */
          } /* if */
          break;
        case 33:
          if ("\0\0\0\0@\f\004\0\0\0\0\0\377\377\0\0"
              "\b\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"[ch >> 3 & 31] &
              1 << (ch & 7)) {
            switch (ch) {
                case 0x2126: ch = 0x03c9; break;
                case 0x212a: ch = 0x006b; break;
                case 0x212b: ch = 0x00e5; break;
                case 0x2132: ch += 28;    break;
                case 0x2183: ch += 1;     break;
              default: ch += 16;          break;
            } /* switch */
          } /* if */
          break;
        case 36:
          if (ch >= 0x24b6 && ch <= 0x24cf) {
            ch += 26;
          } /* if */
          break;
        case 44:
          if ("\377\377\377\377\377\177\0\0\0\0\0\0\235\352%\300"
              "UUUUUUUUUUUU\005(\004\0"[ch >> 3 & 31] &
              1 << (ch & 7)) {
            if (ch <= 0x2c2e) {
              ch += 48;
            } else {
              switch (ch) {
                case 0x2c62: ch = 0x026b; break;
                case 0x2c63: ch = 0x1d7d; break;
                case 0x2c64: ch = 0x027d; break;
                case 0x2c6d: ch = 0x0251; break;
                case 0x2c6e: ch = 0x0271; break;
                case 0x2c6f: ch = 0x0250; break;
                case 0x2c70: ch = 0x0252; break;
                case 0x2c7e: ch = 0x023f; break;
                case 0x2c7f: ch = 0x0240; break;
                default: ch += 1; break;
              } /* switch */
            } /* if */
          } /* if */
          break;
        case 166:
          if ("\0\0\0\0\0\0\0\0UUUUU\025\0\0"
              "UUU\0\0\0\0\0\0\0\0\0\0\0\0\0"[ch >> 3 & 31] &
              1 << (ch & 7)) {
            ch += 1;
          } /* if */
          break;
        case 167:
          if ("\0\0\0\0TUTUUUUUUU\0j"
              "U(\005\0U\005\0\0\0\0\0\0\0\0\0\0"[ch >> 3 & 31] &
              1 << (ch & 7)) {
            if (ch == 0xa77d) {
              ch = 0x1d79;
            } else if (ch == 0xa78d) {
              ch = 0x0265;
            } else if (ch == 0xa7aa) {
              ch = 0x0266;
            } else {
              ch += 1;
            } /* if */
          } /* if */
          break;
        case 255:
          if (ch >= 0xff21 && ch <= 0xff3a) {
            ch += 32;
          } /* if */
          break;
        case 260:
          if (ch >= 0x10400 && ch <= 0x10427) {
            ch += 40;
          } /* if */
          break;
        default:
          break;
      } /* switch */
      dest[pos] = ch;
    } /* for */
  } /* toLower */



static const strElemType toUpperTable2[] = {
    0x2c6f, 0x2c6d, 0x2c70, 0x0181, 0x0186,      0, 0x0189, 0x018a,      0, 0x018f,
         0, 0x0190,      0,      0,      0,      0, 0x0193,      0,      0, 0x0194,
         0, 0xa78d, 0xa7aa,      0, 0x0197, 0x0196,      0, 0x2c62,      0,      0,
         0, 0x019c,      0, 0x2c6e, 0x019d,      0,      0, 0x019f,      0,      0,
         0,      0,      0,      0,      0, 0x2c64,      0,      0, 0x01a6,      0,
         0, 0x01a9,      0,      0,      0,      0, 0x01ae, 0x0244, 0x01b1, 0x01b2,
    0x0245,      0,      0,      0,      0,      0, 0x01b7
  };


void toUpper (const strElemType *const stri, memSizeType length,
    strElemType *const dest)

  {
    memSizeType pos;
    strElemType ch;

  /* toUpper */
    for (pos = 0; pos < length; pos++) {
      ch = stri[pos];
      switch (ch >> 8) {
        case 0:
          if (ch <= '\177') {
            ch = (strElemType) ((unsigned char)
                "\0\1\2\3\4\5\6\7\10\11\12\13\14\15\16\17"
                "\20\21\22\23\24\25\26\27\30\31\32\33\34\35\36\37"
                " !\"#$%&'()*+,-./0123456789:;<=>?"
                "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
                "`ABCDEFGHIJKLMNOPQRSTUVWXYZ{|}~\177"[ch]);
          } else if ("\0\0\0\0\0\0\0\0\0\0\0\0\376\377\377\007"
              "\0\0\0\0\0\0 \0\0\0\0\0\377\377\177\377"[ch >> 3 & 31] &
              1 << (ch & 7)) {
            if (ch == 0x00b5) {
              ch = 0x039c;
            } else if (ch == 0x00ff) {
              ch = 0x0178;
            } else {
              ch -= 32;
            } /* if */
          } /* if */
          break;
        case 1:
          if ("\252\252\252\252\252\252\252TU\251\252\252\252\252\252\324"
              ")\021$F*!Q\242`[U\265\252\252,\252"[ch >> 3 & 31] &
              1 << (ch & 7)) {
            if (ch == 0x0131) {
              ch = 0x0049;
            } else if (ch == 0x017f) {
              ch = 0x0053;
            } else if (ch >= 0x0180 && ch <= 0x019e) {
              ch = (strElemType) ((unsigned char)
                  "\303\0\0\002\0\004\0\0\007\0"
                  "\0\0\013\0\0\0\0\0\021\0"
                  "\0\166\0\0\0\030\275\0\0\0\240"[ch - 0x0180] + 0x0180);
            } else if (ch >= 0x01bf && ch <= 0x01cc) {
              ch = (strElemType) ((unsigned char)
                  "\367\0\0\0\0\0\304\304\0\307"
                  "\307\0\312\312"[ch - 0x01bf] + 0x0100);
            } else if (ch == 0x01dd) {
              ch = 0x018e;
            } else if (ch == 0x01f3) {
              ch = 0x01f1;
            } else {
              ch -= 1;
            } /* if */
          } /* if */
          break;
        case 2:
          if ("\252\252\252\252\250\252\n\220\205\252\337\ni\213& "
              "\t\037\004\0\0\0\0\0\0\0\0\0\0\0\0\0"[ch >> 3 & 31] &
              1 << (ch & 7)) {
            if (ch <= 0x024f) {
              if (ch == 0x023f) {
                ch = 0x2c7e;
              } else if (ch == 0x0240) {
                ch = 0x2c7f;
              } else {
                ch -= 1;
              } /* if */
            } else {
              ch = toUpperTable2[ch - 0x0250];
            } /* if */
          } /* if */
          break;
        case 3:
          if ("\0\0\0\0\0\0\0\0 \0\0\0\0\0\2128"
              "\0\0\0\0\0\360\376\377\377\177\343\252\252\252'\t"[ch >> 3 & 31] &
              1 << (ch & 7)) {
            if (ch <= 0x03af) {
              if (ch == 0x0345) {
                ch += 84;
              } else if (ch <= 0x0377) {
                ch -= 1;
              } else if (ch <= 0x037d) {
                ch += 130;
              } else if (ch == 0x03ac) {
                ch -= 38;
              } else {
                ch -= 37;
              } /* if */
            } else if (ch <= 0x03cb) {
              if (ch == 0x03c2) {
                ch -= 31;
              } else {
                ch -= 32;
              } /* if */
            } else if (ch <= 0x03d7) {
              ch = (strElemType) ((unsigned char)
                  "\214\216\217\0\222\230\0\0\0\246\240\317"[ch - 0x03cc] + 0x0300);
            } else if (ch <= 0x03ef) {
              ch -= 1;
            } else {
              ch = (strElemType) ((unsigned char)
                  "\232\241\371\0\0\225\0\0\367\0\0\372"[ch - 0x03f0] + 0x0300);
            } /* if */
          } /* if */
          break;
        case 4:
          if ("\0\0\0\0\0\0\377\377\377\377\377\377\252\252\252\252"
              "\002\250\252\252\252\252\252\252T\325\252\252\252\252\252\252"[ch >> 3 & 31] &
              1 << (ch & 7)) {
            if (ch <= 0x044f) {
              ch -= 32;
            } else if (ch <= 0x045f) {
              ch -= 80;
            } else if (ch == 0x04cf) {
              ch -= 15;
            } else {
              ch -= 1;
            } /* if */
          } /* if */
          break;
        case 5:
          if ("\252\252\252\252\252\0\0\0\0\0\0\0\376\377\377\377"
              "\177\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"[ch >> 3 & 31] &
              1 << (ch & 7)) {
            if (ch <= 0x0527) {
              ch -= 1;
            } else {
              ch -= 48;
            } /* if */
          } /* if */
          break;
        case 29:
          if (ch == 0x1d79) {
            ch = 0xa77d;
          } else if (ch == 0x1d7d) {
            ch = 0x2c63;
          } /* if */
          break;
        case 30:
          if ("\252\252\252\252\252\252\252\252\252\252\252\252\252\252\252\252"
              "\252\252*\b\252\252\252\252\252\252\252\252\252\252\252\252"[ch >> 3 & 31] &
              1 << (ch & 7)) {
            if (ch == 0x1e9b) {
              ch = 0x1e60;
            } else {
              ch -= 1;
            } /* if */
          } /* if */
          break;
        case 31:
          if ("\377\0?\0\377\0\377\0?\0\252\0\377\0\377?"
              "\377\0\377\0\377\0\013@\b\0\003\0#\0\b\0"[ch >> 3 & 31] &
              1 << (ch & 7)) {
            if (ch >= 0x1f70 && ch <= 0x1f7d) {
              ch = (strElemType) ((unsigned char)
                  "\272\273\310\311\312\313\332\333\370\371\352\353\372\373"[ch - 0x1f70] + 0x1f00);
            } else if (ch == 0x1fb3 || ch == 0x1fc3 || ch == 0x1ff3) {
              ch += 9;
            } else if (ch == 0x1fe5) {
              ch += 7;
            } else if (ch == 0x1fbe) {
              ch = 0x0399;
            } else {
              ch += 8;
            } /* if */
          } /* if */
          break;
        case 33:
          if ("\0\0\0\0\0\0\0\0\0@\0\0\0\0\377\377"
              "\020\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"[ch >> 3 & 31] &
              1 << (ch & 7)) {
            if (ch == 0x214e) {
              ch -= 28;
            } else if (ch == 0x2184) {
              ch -= 1;
            } else {
              ch -= 16;
            } /* if */
          } /* if */
          break;
        case 36:
          if (ch >= 0x24d0 && ch <= 0x24e9) {
            ch -= 26;
          } /* if */
          break;
        case 44:
          if ("\0\0\0\0\0\0\377\377\377\377\377\177b\025H\0"
              "\252\252\252\252\252\252\252\252\252\252\252\252\nP\b\0"[ch >> 3 & 31] &
              1 << (ch & 7)) {
            if (ch <= 0x2c5e) {
              ch -= 48;
            } else if (ch == 0x2c65) {
              ch = 0x023a;
            } else if (ch == 0x2c66) {
              ch = 0x023e;
            } else {
              ch -= 1;
            } /* if */
          } /* if */
          break;
        case 45:
          if (ch >= 0x2d00 && ch <= 0x2d2d) {
            ch -= 0x1c60;
          } /* if */
          break;
        case 166:
          if ("\0\0\0\0\0\0\0\0\252\252\252\252\252*\0\0"
              "\252\252\252\0\0\0\0\0\0\0\0\0\0\0\0\0"[ch >> 3 & 31] &
              1 << (ch & 7)) {
            ch -= 1;
          } /* if */
          break;
        case 167:
          if ("\0\0\0\0\250\252\250\252\252\252\252\252\252\252\0\224"
              "\252\020\n\0\252\002\0\0\0\0\0\0\0\0\0\0"[ch >> 3 & 31] &
              1 << (ch & 7)) {
            ch -= 1;
          } /* if */
          break;
        case 255:
          if (ch >= 0xff41 && ch <= 0xff5a) {
            ch -= 32;
          } /* if */
          break;
        case 260:
          if (ch >= 0x10428 && ch <= 0x1044f) {
            ch -= 40;
          } /* if */
          break;
        default:
          break;
      } /* switch */
      dest[pos] = ch;
    } /* for */
  } /* toUpper */



static rtlArrayType add_stri_to_array (const strElemType *const stri_elems,
    const memSizeType length, rtlArrayType work_array, intType *used_max_position)

  {
    striType new_stri;
    rtlArrayType resized_work_array;
    memSizeType position;

  /* add_stri_to_array */
    if (ALLOC_STRI_SIZE_OK(new_stri, length)) {
      new_stri->size = length;
      memcpy(new_stri->mem, stri_elems, length * sizeof(strElemType));
      if (*used_max_position >= work_array->max_position) {
        if (work_array->max_position >= MAX_MEM_INDEX - 256) {
          resized_work_array = NULL;
        } else {
          resized_work_array = REALLOC_RTL_ARRAY(work_array,
              (uintType) work_array->max_position, (uintType) work_array->max_position + 256);
        } /* if */
        if (resized_work_array == NULL) {
          FREE_STRI(new_stri, new_stri->size);
          new_stri = NULL;
        } else {
          work_array = resized_work_array;
          COUNT3_RTL_ARRAY((uintType) work_array->max_position, (uintType) work_array->max_position + 256);
          work_array->max_position += 256;
        } /* if */
      } /* if */
    } /* if */
    if (new_stri != NULL) {
      work_array->arr[*used_max_position].value.striValue = new_stri;
      (*used_max_position)++;
    } else {
      for (position = 0; position < (uintType) *used_max_position; position++) {
        FREE_STRI(work_array->arr[position].value.striValue,
            work_array->arr[position].value.striValue->size);
      } /* for */
      FREE_RTL_ARRAY(work_array, (uintType) work_array->max_position);
      work_array = NULL;
    } /* if */
    return work_array;
  } /* add_stri_to_array */



striType concat_path (const const_striType absolutePath,
    const const_striType relativePath)

  {
    memSizeType abs_path_length;
    memSizeType estimated_result_size;
    strElemType *abs_path_end;
    const strElemType *rel_path_pos;
    const strElemType *rel_path_beyond;
    memSizeType result_size;
    striType resized_result;
    striType result;

  /* concat_path */
#ifdef TRACE_STR_RTL
    printf("concat_path(");
    prot_stri(absolutePath);
    printf(", ");
    prot_stri(relativePath);
    printf(")\n");
#endif
    /* absolutePath->mem[0] is always '/'. */
    if (absolutePath->size == 1) {
      abs_path_length = 0;
    } else {
      abs_path_length = absolutePath->size;
    } /* if */
    if (unlikely(abs_path_length > MAX_STRI_LEN - relativePath->size - 2)) {
      result = NULL;
    } else {
      /* There is one slash (/) between the two paths. Temporarily    */
      /* there is also a slash at the end of the intermediate result. */
      estimated_result_size = abs_path_length + relativePath->size + 2;
      if (ALLOC_STRI_SIZE_OK(result, estimated_result_size)) {
        memcpy(result->mem, absolutePath->mem, abs_path_length * sizeof(strElemType));
        result->mem[abs_path_length] = '/';
        abs_path_end = &result->mem[abs_path_length];
        rel_path_pos = relativePath->mem;
        rel_path_beyond = &relativePath->mem[relativePath->size];
        while (rel_path_pos < rel_path_beyond) {
          if (&rel_path_pos[1] < rel_path_beyond &&
              rel_path_pos[0] == '.' && rel_path_pos[1] == '.' &&
              (&rel_path_pos[2] >= rel_path_beyond || rel_path_pos[2] == '/')) {
            rel_path_pos += 2;
            if (abs_path_end > result->mem) {
              do {
                abs_path_end--;
              } while (*abs_path_end != '/');
            } /* if */
          } else if (&rel_path_pos[0] < rel_path_beyond &&
                     rel_path_pos[0] == '.' &&
                     (&rel_path_pos[1] >= rel_path_beyond || rel_path_pos[1] == '/')) {
            rel_path_pos++;
          } else if (*rel_path_pos == '/') {
            rel_path_pos++;
          } else {
            do {
              abs_path_end++;
              *abs_path_end = *rel_path_pos;
              rel_path_pos++;
            } while (&rel_path_pos[0] < rel_path_beyond && rel_path_pos[0] != '/');
            abs_path_end++;
            /* The line below adds a temporary slash (/) to the end   */
            /* of the intermediate result. Therefore + 2 is used to   */
            /* compute the estimated_result_size.                     */
            *abs_path_end = '/';
          } /* if */
        } /* while */
        if (abs_path_end == result->mem) {
          result->mem[0] = '/';
          result_size = 1;
        } else {
          result_size = (memSizeType) (abs_path_end - result->mem);
        } /* if */
        REALLOC_STRI_SIZE_SMALLER(resized_result, result, estimated_result_size, result_size);
        if (unlikely(resized_result == NULL)) {
          FREE_STRI(result, estimated_result_size);
          result = NULL;
        } else {
          result = resized_result;
          COUNT3_STRI(estimated_result_size, result_size);
          result->size = result_size;
        } /* if */
      } /* if */
    } /* if */
#ifdef TRACE_STR_RTL
    printf("concat_path --> ");
    prot_stri(result);
    printf("\n");
#endif
    return result;
  } /* concat_path */



#ifdef ALLOW_STRITYPE_SLICES
/**
 *  Append the string 'extension' to 'destination'.
 *  @exception MEMORY_ERROR Not enough memory for the concatenated
 *             string.
 */
void strAppend (striType *const destination, const_striType extension)

  {
    memSizeType new_size;
    striType stri_dest;
    striType new_stri;
    memSizeType extension_size;
    const strElemType *extension_mem;
    const strElemType *extension_origin;

  /* strAppend */
#ifdef TRACE_STR_RTL
    printf("strAppend(");
    prot_stri(*destination);
    printf(", ");
    prot_stri(extension);
    printf(")");
    prot_flush();
#endif
    stri_dest = *destination;
    extension_size = extension->size;
    extension_mem = extension->mem;
    if (unlikely(stri_dest->size > MAX_STRI_LEN - extension_size)) {
      /* number of bytes does not fit into memSizeType */
      raise_error(MEMORY_ERROR);
    } else {
      new_size = stri_dest->size + extension_size;
#ifdef WITH_STRI_CAPACITY
      if (new_size > stri_dest->capacity) {
        if (SLICE_OVERLAPPING(extension, stri_dest)) {
          extension_origin = stri_dest->mem;
        } else {
          extension_origin = NULL;
        } /* if */
        new_stri = growStri(stri_dest, new_size);
        if (unlikely(new_stri == NULL)) {
          raise_error(MEMORY_ERROR);
          return;
        } else {
          if (extension_origin != NULL) {
            /* It is possible that 'extension' is identical to    */
            /* 'stri_dest' or a slice of it. This can be checked  */
            /* with the origin. In this case 'extension_mem' must */
            /* be corrected after realloc() enlarged 'stri_dest'. */
            extension_mem = &new_stri->mem[extension_mem - extension_origin];
            /* Correcting extension->mem is not necessary, since  */
            /* a slice will not be used afterwards. In case when  */
            /* 'extension is identical to 'stri_dest' changing    */
            /* extension->mem is dangerous since 'extension'      */
            /* could have been released.                          */
          } /* if */
          stri_dest = new_stri;
          *destination = stri_dest;
        } /* if */
      } /* if */
      COUNT3_STRI(stri_dest->size, new_size);
      memcpy(&stri_dest->mem[stri_dest->size], extension_mem,
          extension_size * sizeof(strElemType));
      stri_dest->size = new_size;
#else
      if (SLICE_OVERLAPPING(extension, stri_dest)) {
        extension_origin = stri_dest->mem;
      } else {
        extension_origin = NULL;
      } /* if */
      GROW_STRI(new_stri, stri_dest, stri_dest->size, new_size);
      if (unlikely(new_stri == NULL)) {
        raise_error(MEMORY_ERROR);
      } else {
        if (extension_origin != NULL) {
          /* It is possible that 'extension' is identical to    */
          /* 'stri_dest' or a slice of it. This can be checked  */
          /* with the origin. In this case 'extension_mem' must */
          /* be corrected after realloc() enlarged 'stri_dest'. */
          extension_mem = &new_stri->mem[extension_mem - extension_origin];
          /* Correcting extension->mem is not necessary, since  */
          /* a slice will not be used afterwards. In case when  */
          /* 'extension is identical to 'stri_dest' changing    */
          /* extension->mem is dangerous since 'extension'      */
          /* could have been released.                          */
        } /* if */
        COUNT3_STRI(new_stri->size, new_size);
        memcpy(&new_stri->mem[new_stri->size], extension_mem,
            extension_size * sizeof(strElemType));
        new_stri->size = new_size;
        *destination = new_stri;
      } /* if */
#endif
    } /* if */
#ifdef TRACE_STR_RTL
    printf(" --> ");
    prot_stri(*destination);
    printf("\n");
#endif
  } /* strAppend */

#else



/**
 *  Append the string 'extension' to 'destination'.
 *  @exception MEMORY_ERROR Not enough memory for the concatenated
 *             string.
 */
void strAppend (striType *const destination, const_striType extension)

  {
    memSizeType new_size;
    striType stri_dest;
    striType new_stri;

  /* strAppend */
#ifdef TRACE_STR_RTL
    printf("strAppend(");
    prot_stri(*destination);
    printf(", ");
    prot_stri(extension);
    printf(")");
    prot_flush();
#endif
    stri_dest = *destination;
    if (unlikely(stri_dest->size > MAX_STRI_LEN - extension->size)) {
      /* number of bytes does not fit into memSizeType */
      raise_error(MEMORY_ERROR);
    } else {
      new_size = stri_dest->size + extension->size;
#ifdef WITH_STRI_CAPACITY
      if (new_size > stri_dest->capacity) {
        new_stri = growStri(stri_dest, new_size);
        if (unlikely(new_stri == NULL)) {
          raise_error(MEMORY_ERROR);
          return;
        } else {
          if (stri_dest == extension) {
            /* It is possible that stri_dest == extension holds. */
            /* In this case 'extension' must be corrected        */
            /* after realloc() enlarged 'stri_dest'.             */
            extension = new_stri;
          } /* if */
          stri_dest = new_stri;
          *destination = stri_dest;
        } /* if */
      } /* if */
      COUNT3_STRI(stri_dest->size, new_size);
      memcpy(&stri_dest->mem[stri_dest->size], extension->mem,
          extension->size * sizeof(strElemType));
      stri_dest->size = new_size;
#else
      GROW_STRI(new_stri, stri_dest, stri_dest->size, new_size);
      if (unlikely(new_stri == NULL)) {
        raise_error(MEMORY_ERROR);
      } else {
        if (stri_dest == extension) {
          /* It is possible that stri_dest == extension holds. */
          /* In this case 'extension' must be corrected        */
          /* after realloc() enlarged 'stri_dest'.             */
          extension = new_stri;
        } /* if */
        COUNT3_STRI(new_stri->size, new_size);
        memcpy(&new_stri->mem[new_stri->size], extension->mem,
            extension->size * sizeof(strElemType));
        new_stri->size = new_size;
        *destination = new_stri;
      } /* if */
#endif
    } /* if */
#ifdef TRACE_STR_RTL
    printf(" --> ");
    prot_stri(*destination);
    printf("\n");
#endif
  } /* strAppend */

#endif



/**
 *  Append the string 'extension' to 'destination'.
 *  StrAppendTemp is used by the compiler when 'extension' is temporary
 *  value that can be reused.
 *  @exception MEMORY_ERROR Not enough memory for the concatenated
 *             string.
 */
void strAppendTemp (striType *const destination, const striType extension)

  {
    memSizeType new_size;
    striType stri_dest;

  /* strAppendTemp */
#ifdef TRACE_STR_RTL
    printf("strAppendTemp(");
    prot_stri(*destination);
    printf(", ");
    prot_stri(extension);
    printf(")");
    prot_flush();
#endif
    stri_dest = *destination;
    if (unlikely(stri_dest->size > MAX_STRI_LEN - extension->size)) {
      /* number of bytes does not fit into memSizeType */
      raise_error(MEMORY_ERROR);
    } else {
      new_size = stri_dest->size + extension->size;
#ifdef WITH_STRI_CAPACITY
      if (new_size <= stri_dest->capacity) {
        COUNT3_STRI(stri_dest->size, new_size);
        memcpy(&stri_dest->mem[stri_dest->size], extension->mem,
            extension->size * sizeof(strElemType));
        stri_dest->size = new_size;
        FREE_STRI(extension, extension->size);
      } else if (new_size <= extension->capacity) {
        if (stri_dest->size != 0) {
          COUNT3_STRI(extension->size, new_size);
          memmove(&extension->mem[stri_dest->size], extension->mem,
              extension->size * sizeof(strElemType));
          memcpy(extension->mem, stri_dest->mem,
              stri_dest->size * sizeof(strElemType));
          extension->size = new_size;
        } /* if */
        *destination = extension;
        FREE_STRI(stri_dest, stri_dest->size);
      } else {
        stri_dest = growStri(stri_dest, new_size);
        if (unlikely(stri_dest == NULL)) {
          FREE_STRI(extension, extension->size);
          raise_error(MEMORY_ERROR);
        } else {
          *destination = stri_dest;
          COUNT3_STRI(stri_dest->size, new_size);
          memcpy(&stri_dest->mem[stri_dest->size], extension->mem,
              extension->size * sizeof(strElemType));
          stri_dest->size = new_size;
          FREE_STRI(extension, extension->size);
        } /* if */
      } /* if */
#else
      GROW_STRI(stri_dest, stri_dest, stri_dest->size, new_size);
      if (unlikely(stri_dest == NULL)) {
        FREE_STRI(extension, extension->size);
        raise_error(MEMORY_ERROR);
      } else {
        *destination = stri_dest;
        COUNT3_STRI(stri_dest->size, new_size);
        memcpy(&stri_dest->mem[stri_dest->size], extension->mem,
            extension->size * sizeof(strElemType));
        stri_dest->size = new_size;
        FREE_STRI(extension, extension->size);
      } /* if */
#endif
    } /* if */
#ifdef TRACE_STR_RTL
    printf(" --> ");
    prot_stri(*destination);
    printf("\n");
#endif
  } /* strAppendTemp */



#ifdef OUT_OF_ORDER
rtlArrayType strChEscSplit (const const_striType mainStri, const charType delimiter,
    const charType escape)

  {
    intType used_max_position;
    const strElemType *search_start;
    const strElemType *search_end;
    const strElemType *curr_pos;
    const strElemType *found_pos;
    striType curr_stri;
    const strElemType *stri_pos;
    memSizeType pos;
    rtlArrayType resized_result_array;
    rtlArrayType result_array;

  /* strChEscSplit */
    if (unlikely(delimiter == escape)) {
      raise_error(RANGE_ERROR);
    } else {
      if (ALLOC_RTL_ARRAY(result_array, 256)) {
        result_array->min_position = 1;
        result_array->max_position = 256;
        used_max_position = 0;
        search_start = mainStri->mem;
        search_end = &mainStri->mem[mainStri->size];
        old_pos = search_start;
        curr_pos = search_start;
        while (curr_pos != search_end && result_array != NULL) {
          while (curr_pos != search_end && *curr_pos != delimiter) {
            while (curr_pos != search_end && *curr_pos != delimiter && *curr_pos != escape) {
              curr_pos++;
            } /* while */
            memcpy(stri_pos, old_pos, (memSizeType) (curr_pos - old_pos));
            stri_pos += curr_pos - old_pos;
            if (curr_pos != search_end && *curr_pos == escape) {
              curr_pos++;
              if (curr_pos != search_end) {
                *stri_pos = *curr_pos;
                stri_pos++;
              } /* if */
            } /* if */
          } /* while */
          result_array = add_stri_to_array(search_start,
              (memSizeType) (found_pos - search_start), result_array,
              &used_max_position);
          search_start = found_pos + 1;

      if (result_array != NULL) {
        result_array = add_stri_to_array(search_start,
            (memSizeType) (search_end - search_start), result_array,
            &used_max_position);
        if (result_array != NULL) {
          resized_result_array = REALLOC_RTL_ARRAY(result_array,
              (uintType) result_array->max_position, (uintType) used_max_position);
          if (resized_result_array == NULL) {
            for (pos = 0; pos < (uintType) used_max_position; pos++) {
              FREE_STRI(result_array->arr[pos].value.striValue,
                  result_array->arr[pos].value.striValue->size);
            } /* for */
            FREE_RTL_ARRAY(result_array, (uintType) result_array->max_position);
            result_array = NULL;
          } else {
            result_array = resized_result_array;
            COUNT3_RTL_ARRAY((uintType) result_array->max_position, (uintType) used_max_position);
            result_array->max_position = used_max_position;
          } /* if */
        } /* if */
      } /* if */
    } /* if */
    if (unlikely(result_array == NULL)) {
      raise_error(MEMORY_ERROR);
    } /* if */
    return result_array;
  } /* strChEscSplit */
#endif



/**
 *  Search char 'searched' in 'mainStri' at or after 'fromIndex'.
 *  The search starts at 'fromIndex' and proceeds to the right.
 *  The first character in a string has the position 1.
 *  @return the position of 'searched' or 0 when 'mainStri'
 *          does not contain 'searched' at or after 'fromIndex'.
 *  @exception RANGE_ERROR 'fromIndex' <= 0 holds.
 */
intType strChIPos (const const_striType mainStri, const charType searched,
    const intType fromIndex)

  {
    const strElemType *main_mem;
    const strElemType *found_pos;

  /* strChIPos */
    if (unlikely(fromIndex <= 0)) {
      raise_error(RANGE_ERROR);
    } else {
      if ((uintType) fromIndex <= mainStri->size) {
        main_mem = mainStri->mem;
        found_pos = search_strelem(&main_mem[fromIndex - 1], searched,
            &main_mem[mainStri->size]);
        if (found_pos != NULL) {
          return ((intType) (found_pos - main_mem)) + 1;
        } /* if */
      } /* if */
    } /* if */
    return 0;
  } /* strChIPos */



striType strChMult (const charType ch, const intType factor)

  {
    striType result;

  /* strChMult */
#ifdef TRACE_STR_RTL
    printf("strChMult('\\" FMT_U32 ";', " FMT_D ")\n", ch, factor);
#endif
    if (unlikely(factor < 0)) {
      raise_error(RANGE_ERROR);
      result = NULL;
    } else {
      if (unlikely((uintType) factor > MAX_STRI_LEN ||
                   !ALLOC_STRI_SIZE_OK(result, (memSizeType) factor))) {
        raise_error(MEMORY_ERROR);
        result = NULL;
      } else {
        result->size = (memSizeType) factor;
        memset_to_strelem(result->mem, ch, (memSizeType) factor);
      } /* if */
    } /* if */
    return result;
  } /* strChMult */



/**
 *  Determine leftmost position of char 'searched' in 'mainStri'.
 *  The first character in a string has the position 1.
 *  @return the position of 'searched' or 0 when 'mainStri'
 *          does not contain 'searched'.
 */
intType strChPos (const const_striType mainStri, const charType searched)

  {
    const strElemType *main_mem;
    const strElemType *found_pos;

  /* strChPos */
    if (mainStri->size >= 1) {
      main_mem = mainStri->mem;
      found_pos = search_strelem(main_mem, searched, &main_mem[mainStri->size]);
      if (found_pos != NULL) {
        return ((intType) (found_pos - main_mem)) + 1;
      } /* if */
    } /* if */
    return 0;
  } /* strChPos */



rtlArrayType strChSplit (const const_striType mainStri, const charType delimiter)

  {
    intType used_max_position;
    const strElemType *search_start;
    const strElemType *search_end;
    const strElemType *found_pos;
    memSizeType pos;
    rtlArrayType resized_result_array;
    rtlArrayType result_array;

  /* strChSplit */
    if (ALLOC_RTL_ARRAY(result_array, 256)) {
      result_array->min_position = 1;
      result_array->max_position = 256;
      used_max_position = 0;
      search_start = mainStri->mem;
      search_end = &mainStri->mem[mainStri->size];
      while ((found_pos = search_strelem(search_start, delimiter, search_end)) != NULL &&
          result_array != NULL) {
        result_array = add_stri_to_array(search_start,
            (memSizeType) (found_pos - search_start), result_array,
            &used_max_position);
        search_start = found_pos + 1;
      } /* while */
      if (result_array != NULL) {
        result_array = add_stri_to_array(search_start,
            (memSizeType) (search_end - search_start), result_array,
            &used_max_position);
        if (result_array != NULL) {
          resized_result_array = REALLOC_RTL_ARRAY(result_array,
              (uintType) result_array->max_position, (uintType) used_max_position);
          if (resized_result_array == NULL) {
            for (pos = 0; pos < (uintType) used_max_position; pos++) {
              FREE_STRI(result_array->arr[pos].value.striValue,
                  result_array->arr[pos].value.striValue->size);
            } /* for */
            FREE_RTL_ARRAY(result_array, (uintType) result_array->max_position);
            result_array = NULL;
          } else {
            result_array = resized_result_array;
            COUNT3_RTL_ARRAY((uintType) result_array->max_position, (uintType) used_max_position);
            result_array->max_position = used_max_position;
          } /* if */
        } /* if */
      } /* if */
    } /* if */
    if (unlikely(result_array == NULL)) {
      raise_error(MEMORY_ERROR);
    } /* if */
    return result_array;
  } /* strChSplit */



striType strCLit (const const_striType stri)

  {
    register strElemType character;
    register memSizeType position;
    memSizeType striSize;
    memSizeType pos;
    striType resized_result;
    striType result;

  /* strCLit */
#ifdef TRACE_STR_RTL
    printf("strCLit(");
    prot_stri(stri);
    printf(")\n");
#endif
    striSize = stri->size;
    if (unlikely(striSize > (MAX_STRI_LEN - 2) / 4 ||
                 !ALLOC_STRI_SIZE_OK(result, 4 * striSize + 2))) {
      raise_error(MEMORY_ERROR);
      result = NULL;
    } else {
      result->mem[0] = (strElemType) '"';
      pos = 1;
      for (position = 0; position < striSize; position++) {
        character = stri->mem[position];
        /* The following comparisons use int literals like 255      */
        /* instead of char literals like '\377'. When char literals */
        /* are signed (this is C implementation dependent) the      */
        /* integral promotion (conversion to int) triggers a sign   */
        /* extension. In this case the sign extension of '\377'     */
        /* leads to the int value -1 instead of the desired 255.    */
        if (character < 127) {
          if (character < ' ') {
            result->mem[pos] = (strElemType) '\\';
            if (cstri_escape_sequence[character][1] == '0') {
              /* Always write three octal digits to avoid errors when */
              /* the octal representation is followed by a digit.     */
              result->mem[pos + 1] = (strElemType) '0';
              /* Write the character as two octal digits. */
              /* This code is much faster than sprintf(). */
              result->mem[pos + 2] = (strElemType) ((character >> 3 & 0x7) + '0');
              result->mem[pos + 3] = (strElemType) ((character      & 0x7) + '0');
              pos += 4;
            } else {
              result->mem[pos + 1] = (strElemType) cstri_escape_sequence[character][1];
              pos += 2;
            } /* if */
#ifdef TRIGRAPH_SEQUENCES_ARE_REPLACED
          } else if (character == '\\' || character == '\"' ||
              (character == '?' && position >= 1 && stri->mem[position - 1] == '?')) {
#else
          } else if (character == '\\' || character == '\"') {
#endif
            result->mem[pos]     = (strElemType) '\\';
            result->mem[pos + 1] = character;
            pos += 2;
          } else {
            result->mem[pos]     = character;
            pos++;
          } /* if */
        } else if (character < 256) {
          result->mem[pos]     = (strElemType) '\\';
          /* Write the character as three octal digits. */
          /* This code is much faster than sprintf().   */
          result->mem[pos + 1] = (strElemType) ((character >> 6 & 0x7) + '0');
          result->mem[pos + 2] = (strElemType) ((character >> 3 & 0x7) + '0');
          result->mem[pos + 3] = (strElemType) ((character      & 0x7) + '0');
          pos += 4;
        } else {
          FREE_STRI(result, 4 * striSize + 2);
          raise_error(RANGE_ERROR);
          return NULL;
        } /* if */
      } /* for */
      result->mem[pos] = (strElemType) '"';
      pos++;
      result->size = pos;
      REALLOC_STRI_SIZE_SMALLER(resized_result, result, 4 * striSize + 2, pos);
      if (unlikely(resized_result == NULL)) {
        FREE_STRI(result, 4 * striSize + 2);
        raise_error(MEMORY_ERROR);
        result = NULL;
      } else {
        result = resized_result;
        COUNT3_STRI(4 * striSize + 2, pos);
      } /* if */
    } /* if */
    return result;
  } /* strCLit */



/**
 *  Compare two strings.
 *  @return -1, 0 or 1 if the first argument is considered to be
 *          respectively less than, equal to, or greater than the
 *          second.
 */
intType strCompare (const const_striType stri1, const const_striType stri2)

  {
    intType signumValue;

  /* strCompare */
    if (stri1->size < stri2->size) {
      signumValue = strelem_memcmp(stri1->mem, stri2->mem, stri1->size);
      if (signumValue == 0) {
        signumValue = -1;
      } /* if */
    } else {
      signumValue = strelem_memcmp(stri1->mem, stri2->mem, stri2->size);
      if (signumValue == 0 && stri1->size > stri2->size) {
        signumValue = 1;
      } /* if */
    } /* if */
    return signumValue;
  } /* strCompare */



/**
 *  Reinterpret the generic parameters as striType and call strCompare.
 *  Function pointers in C programs generated by the Seed7 compiler
 *  may point to this function. This assures correct behaviour even
 *  when sizeof(genericType) != sizeof(striType).
 */
intType strCmpGeneric (const genericType value1, const genericType value2)

  { /* strCmpGeneric */
    return strCompare(((const_rtlObjectType *) &value1)->value.striValue,
                      ((const_rtlObjectType *) &value2)->value.striValue);
  } /* strCmpGeneric */



/**
 *  Concatenate two strings.
 *  @return the result of the concatenation.
 */
striType strConcat (const const_striType stri1, const const_striType stri2)

  {
    memSizeType result_size;
    striType result;

  /* strConcat */
#ifdef TRACE_STR_RTL
    printf("strConcat(");
    prot_stri(stri1);
    printf(", ");
    prot_stri(stri2);
    printf(")");
    prot_flush();
#endif
    if (unlikely(stri1->size > MAX_STRI_LEN - stri2->size)) {
      /* number of bytes does not fit into memSizeType */
      raise_error(MEMORY_ERROR);
      result = NULL;
    } else {
      result_size = stri1->size + stri2->size;
      if (unlikely(!ALLOC_STRI_SIZE_OK(result, result_size))) {
        raise_error(MEMORY_ERROR);
      } else {
        result->size = result_size;
        memcpy(result->mem, stri1->mem,
            stri1->size * sizeof(strElemType));
        memcpy(&result->mem[stri1->size], stri2->mem,
            stri2->size * sizeof(strElemType));
      } /* if */
    } /* if */
#ifdef TRACE_STR_RTL
    printf("strConcat -> ");
    prot_stri(result);
    printf("\n");
#endif
    return result;
  } /* strConcat */



/**
 *  Concatenate an arbitrary number of strings.
 *  StrConcatN is used by the compiler to optimize the concatination of
 *  three or more strings.
 *  @return the result of the concatenation.
 */
striType strConcatN (const const_striType striArray[], memSizeType arraySize)

  {
    memSizeType pos;
    memSizeType result_size;
    memSizeType size_limit = MAX_STRI_LEN;
    memSizeType elem_size;
    strElemType *dest;
    striType result;

  /* strConcatN */
#ifdef TRACE_STR_RTL
    printf("strConcatN(" FMT_U_MEM ")\n", arraySize);
#endif
    for (pos = arraySize; pos > 0; pos--) {
      /* printf("arr[%lu]->size=%lu\n", pos, striArray[pos - 1]->size);
      printf("arr[%lu]=(%08lx) ", pos, striArray[pos - 1]);
      prot_stri(striArray[pos - 1]);
      printf("\n"); */
      if (unlikely(striArray[pos - 1]->size > size_limit)) {
        raise_error(MEMORY_ERROR);
        return NULL;
      } else {
        size_limit -= striArray[pos - 1]->size;
      } /* if */
    } /* for */
    result_size = MAX_STRI_LEN - size_limit;
    /* printf("result_size=%lu\n", result_size); */
    if (unlikely(!ALLOC_STRI_SIZE_OK(result, result_size))) {
      raise_error(MEMORY_ERROR);
    } else {
      result->size = result_size;
      dest = result->mem;
      for (pos = 0; pos < arraySize; pos++) {
        elem_size = striArray[pos]->size;
        memcpy(dest, striArray[pos]->mem, elem_size * sizeof(strElemType));
        dest += elem_size;
      } /* for */
    } /* if */
#ifdef TRACE_STR_RTL
    printf("strConcatN -> ");
    prot_stri(result);
    printf("\n");
#endif
    return result;
  } /* strConcatN */



/**
 *  Concatenate two strings.
 *  The parameter 'stri1' is resized and 'stri2' is copied to the
 *  enlarged area of 'stri1'. StrConcatTemp is used by the compiler
 *  when 'stri1' is temporary value that can be reused.
 *  @return the resized parameter 'stri1.
 */
striType strConcatTemp (striType stri1, const const_striType stri2)

  {
    memSizeType result_size;
    striType resized_stri1;

  /* strConcatTemp */
    if (unlikely(stri1->size > MAX_STRI_LEN - stri2->size)) {
      /* number of bytes does not fit into memSizeType */
      FREE_STRI(stri1, stri1->size);
      raise_error(MEMORY_ERROR);
      stri1 = NULL;
    } else {
      result_size = stri1->size + stri2->size;
#ifdef WITH_STRI_CAPACITY
      if (result_size > stri1->capacity) {
        resized_stri1 = growStri(stri1, result_size);
        if (unlikely(resized_stri1 == NULL)) {
          FREE_STRI(stri1, stri1->size);
          raise_error(MEMORY_ERROR);
          return NULL;
        } else {
          stri1 = resized_stri1;
        } /* if */
      } /* if */
      COUNT3_STRI(stri1->size, result_size);
      memcpy(&stri1->mem[stri1->size], stri2->mem,
          stri2->size * sizeof(strElemType));
      stri1->size = result_size;
#else
      GROW_STRI(resized_stri1, stri1, stri1->size, result_size);
      if (unlikely(resized_stri1 == NULL)) {
        FREE_STRI(stri1, stri1->size);
        raise_error(MEMORY_ERROR);
        stri1 = NULL;
      } else {
        stri1 = resized_stri1;
        COUNT3_STRI(stri1->size, result_size);
        memcpy(&stri1->mem[stri1->size], stri2->mem,
            stri2->size * sizeof(strElemType));
        stri1->size = result_size;
      } /* if */
#endif
    } /* if */
    return stri1;
  } /* strConcatTemp */



#ifdef ALLOW_STRITYPE_SLICES
void strCopy (striType *const stri_to, const const_striType stri_from)

  {
    memSizeType new_size;
    striType stri_dest;
    const strElemType *stri_from_mem;
#ifndef WITH_STRI_CAPACITY
    const strElemType *stri_from_origin;
#endif

  /* strCopy */
#ifdef TRACE_STR_RTL
    printf("strCopy(");
    prot_stri(*stri_to);
    printf(", ");
    prot_stri(stri_from);
    printf(")");
    prot_flush();
#endif
    /* printf("stri_to=%lu, stri_from=%lu\n", stri_to, stri_from); */
    stri_dest = *stri_to;
    new_size = stri_from->size;
#ifdef WITH_STRI_CAPACITY
    if (stri_dest->capacity >= new_size && !SHRINK_REASON(stri_dest, new_size)) {
      COUNT3_STRI(stri_dest->size, new_size);
      stri_dest->size = new_size;
      stri_from_mem = stri_from->mem;
#else
    if (stri_dest->size > new_size) {
      stri_from_mem = stri_from->mem;
      if (SLICE_OVERLAPPING(stri_from, stri_dest)) {
        stri_from_origin = stri_dest->mem;
      } else {
        stri_from_origin = NULL;
      } /* if */
      SHRINK_STRI(stri_dest, stri_dest, stri_dest->size, new_size);
      /* printf("strCopy(old_size=%lu, new_size=%lu)\n", stri_dest->size, new_size); */
      if (unlikely(stri_dest == NULL)) {
        raise_error(MEMORY_ERROR);
        return;
      } else {
        COUNT3_STRI(stri_dest->size, new_size);
        stri_dest->size = new_size;
        if (stri_from_origin != NULL) {
          /* It is possible that 'stri_from' is identical to    */
          /* '*stri_to' or a slice of it. This can be checked   */
          /* with the origin. In this case 'stri_from_mem' must */
          /* be corrected after realloc() enlarged 'stri_dest'. */
          stri_from_mem = &stri_dest->mem[stri_from_mem - stri_from_origin];
          /* Correcting stri_from->mem is not necessary, since  */
          /* a slice will not be used afterwards. In case when  */
          /* 'stri_from' is identical to '*stri_to' changing    */
          /* stri_from->mem is dangerous since 'stri_from'      */
          /* could have been released.                          */
        } /* if */
        *stri_to = stri_dest;
      } /* if */
#endif
      /* It is possible that stri_dest and stri_from overlap. */
      /* The behavior of memcpy() is undefined when source    */
      /* and destination areas overlap (or are identical).    */
      /* Therefore memmove() is used instead of memcpy().     */
      memmove(stri_dest->mem, stri_from_mem,
          new_size * sizeof(strElemType));
    } else {
      if (unlikely(!ALLOC_STRI_SIZE_OK(stri_dest, new_size))) {
        raise_error(MEMORY_ERROR);
        return;
      } else {
        stri_dest->size = new_size;
        memcpy(stri_dest->mem, stri_from->mem,
            new_size * sizeof(strElemType));
        FREE_STRI(*stri_to, (*stri_to)->size);
        *stri_to = stri_dest;
      } /* if */
    } /* if */
#ifdef TRACE_STR_RTL
    printf(" --> ");
    prot_stri(*stri_to);
    printf("\n");
#endif
  } /* strCopy */

#else


void strCopy (striType *const stri_to, const const_striType stri_from)

  {
    memSizeType new_size;
    striType stri_dest;

  /* strCopy */
#ifdef TRACE_STR_RTL
    printf("strCopy(");
    prot_stri(*stri_to);
    printf(", ");
    prot_stri(stri_from);
    printf(")");
    prot_flush();
#endif
    /* printf("stri_to=%lu, stri_from=%lu\n", stri_to, stri_from); */
    stri_dest = *stri_to;
    /* printf("stri_dest=%lu\n", stri_dest); */
    new_size = stri_from->size;
    if (stri_dest->size != new_size) {
#ifdef WITH_STRI_CAPACITY
      if (stri_dest->capacity >= new_size && !SHRINK_REASON(stri_dest, new_size)) {
        stri_dest->size = new_size;
#else
      if (stri_dest->size > new_size) {
        SHRINK_STRI(stri_dest, stri_dest, stri_dest->size, new_size);
        /* printf("strCopy(old_size=%lu, new_size=%lu)\n", stri_dest->size, new_size); */
        if (unlikely(stri_dest == NULL)) {
          raise_error(MEMORY_ERROR);
          return;
        } else {
          COUNT3_STRI(stri_dest->size, new_size);
          stri_dest->size = new_size;
          *stri_to = stri_dest;
        } /* if */
#endif
      } else {
        if (unlikely(!ALLOC_STRI_SIZE_OK(stri_dest, new_size))) {
          raise_error(MEMORY_ERROR);
          return;
        } else {
          FREE_STRI(*stri_to, (*stri_to)->size);
          stri_dest->size = new_size;
          *stri_to = stri_dest;
        } /* if */
      } /* if */
    } /* if */
    /* It is possible that *stri_to == stri_from holds. The */
    /* behavior of memcpy() is undefined when source and    */
    /* destination areas overlap (or are identical).        */
    /* Therefore memmove() is used instead of memcpy().     */
    memmove(stri_dest->mem, stri_from->mem,
        new_size * sizeof(strElemType));
#ifdef TRACE_STR_RTL
    printf(" --> ");
    prot_stri(*stri_to);
    printf("\n");
#endif
  } /* strCopy */

#endif



void strCpyGeneric (genericType *const dest, const genericType source)

  { /* strCpyGeneric */
    strCopy(&((rtlObjectType *) dest)->value.striValue,
            ((const_rtlObjectType *) &source)->value.striValue);
  } /* strCpyGeneric */



striType strCreate (const const_striType stri_from)

  {
    memSizeType new_size;
    striType result;

  /* strCreate */
    new_size = stri_from->size;
    if (unlikely(!ALLOC_STRI_SIZE_OK(result, new_size))) {
      raise_error(MEMORY_ERROR);
    } else {
      result->size = new_size;
      if (new_size != 0) {
        memcpy(result->mem, stri_from->mem, new_size * sizeof(strElemType));
      } /* if */
    } /* if */
    return result;
  } /* strCreate */



/**
 *  Generic Create function to be used via function pointers.
 *  Function pointers in C programs generated by the Seed7 compiler
 *  may point to this function. This assures correct behaviour even
 *  when sizeof(genericType) != sizeof(striType).
 */
genericType strCreateGeneric (const genericType from_value)

  {
    rtlObjectType result;

  /* strCreateGeneric */
    INIT_GENERIC_PTR(result.value.genericValue);
    result.value.striValue =
        strCreate(((const_rtlObjectType *) &from_value)->value.striValue);
    return result.value.genericValue;
  } /* strCreateGeneric */



void strDestr (const const_striType old_string)

  { /* strDestr */
    /* printf("strDestr(%lX)\n", old_string); */
    if (old_string != NULL) {
      FREE_STRI(old_string, old_string->size);
    } /* if */
  } /* strDestr */



/**
 *  Generic Destr function to be used via function pointers.
 *  Function pointers in C programs generated by the Seed7 compiler
 *  may point to this function. This assures correct behaviour even
 *  when sizeof(genericType) != sizeof(striType).
 */
void strDestrGeneric (const genericType old_value)

  { /* strDestrGeneric */
    strDestr(((const_rtlObjectType *) &old_value)->value.striValue);
  } /* strDestrGeneric */



striType strEmpty (void)

  {
    striType result;

  /* strEmpty */
    if (unlikely(!ALLOC_STRI_SIZE_OK(result, 0))) {
      raise_error(MEMORY_ERROR);
    } else {
      result->size = 0;
    } /* if */
    return result;
  } /* strEmpty */



/**
 *  Check if stri1 is greater than or equal to stri2.
 *  @return TRUE if stri1 is greater than or equal to stri2,
 *          FALSE otherwise.
 */
boolType strGe (const const_striType stri1, const const_striType stri2)

  { /* strGe */
    if (stri1->size >= stri2->size) {
      return strelem_memcmp(stri1->mem, stri2->mem, stri2->size) >= 0;
    } else {
      return strelem_memcmp(stri1->mem, stri2->mem, stri1->size) > 0;
    } /* if */
  } /* strGe */



/**
 *  Check if stri1 is greater than stri2.
 *  @return TRUE if stri1 is greater than stri2,
 *          FALSE otherwise.
 */
boolType strGt (const const_striType stri1, const const_striType stri2)

  { /* strGt */
    if (stri1->size > stri2->size) {
      return strelem_memcmp(stri1->mem, stri2->mem, stri2->size) >= 0;
    } else {
      return strelem_memcmp(stri1->mem, stri2->mem, stri1->size) > 0;
    } /* if */
  } /* strGt */



/**
 *  Compute the hash value of a string.
 *  @return the hash value.
 */
intType strHashCode (const const_striType stri)

  { /* strHashCode */
    return hashCode(stri);
  } /* strHashCode */



#ifdef ALLOW_STRITYPE_SLICES
/**
 *  Get a substring ending at a stop position.
 *  The first character in a string has the position 1.
 *  This function is used by the compiler to avoid copiing string data.
 *  The 'slice' is initialized to refer to the head of 'stri'
 */
void strHeadSlice (const const_striType stri, const intType stop, striType slice)

  {
    memSizeType striSize;

  /* strHeadSlice */
#ifdef TRACE_STR_RTL
    printf("strHeadSlice(");
    prot_stri(stri);
    printf(", " FMT_D ")", stop);
    prot_flush();
#endif
    striSize = stri->size;
    if (stop >= 1 && striSize >= 1) {
      SET_SLICE_CAPACITY(slice, 0);
      slice->mem = stri->mem;
      if (striSize <= (uintType) stop) {
        slice->size = striSize;
      } else {
        slice->size = (memSizeType) stop;
      } /* if */
    } else {
      SET_SLICE_CAPACITY(slice, 0);
      slice->mem = NULL;
      slice->size = 0;
    } /* if */
#ifdef TRACE_STR_RTL
    printf(" --> ");
    prot_stri(slice);
    printf("\n");
#endif
  } /* strHeadSlice */

#endif



/**
 *  Get a substring ending at a stop position.
 *  The first character in a string has the position 1.
 *  @return the substring ending at the stop position.
 *  @exception MEMORY_ERROR Not enough memory to represent the result.
 */
striType strHead (const const_striType stri, const intType stop)

  {
    memSizeType striSize;
    memSizeType result_size;
    striType result;

  /* strHead */
    striSize = stri->size;
    if (stop >= 1 && striSize >= 1) {
      if (striSize <= (uintType) stop) {
        result_size = striSize;
      } else {
        result_size = (memSizeType) stop;
      } /* if */
      if (unlikely(!ALLOC_STRI_SIZE_OK(result, result_size))) {
        raise_error(MEMORY_ERROR);
      } else {
        result->size = result_size;
        memcpy(result->mem, stri->mem, result_size * sizeof(strElemType));
      } /* if */
    } else {
      if (unlikely(!ALLOC_STRI_SIZE_OK(result, (memSizeType) 0))) {
        raise_error(MEMORY_ERROR);
      } else {
        result->size = 0;
      } /* if */
    } /* if */
    return result;
  } /* strHead */



/**
 *  Get a substring ending at a stop position.
 *  The first character in a string has the position 1.
 *  StrHeadTemp is used by the compiler when 'stri' is temporary
 *  value that can be reused.
 *  @return the substring ending at the stop position.
 *  @exception MEMORY_ERROR Not enough memory to represent the result.
 */
striType strHeadTemp (const striType stri, const intType stop)

  {
    memSizeType striSize;
    memSizeType result_size;
    striType result;

  /* strHeadTemp */
    striSize = stri->size;
    if (stop >= 1 && striSize >= 1) {
      if (striSize <= (uintType) stop) {
        return stri;
      } else {
        result_size = (memSizeType) stop;
      } /* if */
    } else {
      result_size = 0;
    } /* if */
#ifdef WITH_STRI_CAPACITY
    if (!SHRINK_REASON(stri, result_size)) {
      COUNT3_STRI(striSize, result_size);
      result = stri;
      result->size = result_size;
    } else {
      result = shrinkStri(stri, result_size);
      if (unlikely(result == NULL)) {
        FREE_STRI(stri, stri->size);
        raise_error(MEMORY_ERROR);
      } else {
        COUNT3_STRI(striSize, result_size);
        result->size = result_size;
      } /* if */
    } /* if */
#else
    SHRINK_STRI(result, stri, striSize, result_size);
    if (unlikely(result == NULL)) {
      FREE_STRI(stri, stri->size);
      raise_error(MEMORY_ERROR);
    } else {
      COUNT3_STRI(striSize, result_size);
      result->size = result_size;
    } /* if */
#endif
    return result;
  } /* strHeadTemp */



/**
 *  Search string 'searched' in 'mainStri' at or before 'fromIndex'.
 *  The search starts at 'fromIndex' and proceeds to the left.
 *  The first character in a string has the position 1.
 *  This function uses a modified Boyer–Moore string search algorithm.
 *  @return the position of 'searched' or 0 when 'mainStri'
 *          does not contain 'searched' at or after 'fromIndex'.
 */
static intType strIPos2 (const const_striType mainStri, const const_striType searched,
    const intType fromIndex)

  {
    memSizeType main_size;
    memSizeType searched_size;
    strElemType ch_n;
    const strElemType *ch_n_pos;
    memSizeType delta;
    memSizeType charDelta[CHAR_DELTA_BEYOND + 1];
    memSizeType pos;
    const strElemType *main_mem;
    const strElemType *searched_mem;
    const strElemType *search_start;
    const strElemType *search_end;

  /* strIPos2 */
    main_size = mainStri->size - ((memSizeType) fromIndex - 1);
    searched_size = searched->size;
    for (ch_n = 0; ch_n <= CHAR_DELTA_BEYOND; ch_n++) {
      charDelta[ch_n] = searched_size;
    } /* for */
    searched_mem = searched->mem;
    for (pos = 0; pos < searched_size - 1; pos++) {
      ch_n = searched_mem[pos];
      if (ch_n < CHAR_DELTA_BEYOND) {
        charDelta[ch_n] = searched_size - pos - 1;
      } else {
        charDelta[CHAR_DELTA_BEYOND] = searched_size - pos - 1;
      } /* if */
    } /* for */
    ch_n = searched_mem[searched_size - 1];
    ch_n_pos = rsearch_strelem(&searched_mem[searched_size - 2], ch_n, searched_size - 1);
    if (ch_n_pos == NULL) {
      delta = searched_size;
    } else {
      delta = (memSizeType) (&searched_mem[searched_size - 1] - ch_n_pos);
    } /* if */
    main_mem = &mainStri->mem[fromIndex - 1];
    search_start = &main_mem[searched_size - 1];
    search_end = &main_mem[main_size];
    while (search_start < search_end) {
      search_start = search_strelem2(search_start, ch_n, search_end, charDelta);
      if (search_start == NULL) {
        return 0;
      } else {
        if (memcmp(search_start - searched_size + 1, searched_mem,
            (searched_size - 1) * sizeof(strElemType)) == 0) {
          return ((intType) (search_start - searched_size + 1 - main_mem)) + fromIndex;
        } else {
          search_start += delta;
        } /* if */
      } /* if */
    } /* while */
    return 0;
  } /* strIPos2 */



/**
 *  Search string 'searched' in 'mainStri' at or before 'fromIndex'.
 *  The search starts at 'fromIndex' and proceeds to the left.
 *  The first character in a string has the position 1.
 *  This function calls strIPos2 when 'mainStri' is long.
 *  @return the position of 'searched' or 0 when 'mainStri'
 *          does not contain 'searched' at or after 'fromIndex'.
 *  @exception RANGE_ERROR 'fromIndex' <= 0 holds.
 */
intType strIPos (const const_striType mainStri, const const_striType searched,
    const intType fromIndex)

  {
    memSizeType main_size;
    memSizeType searched_size;
    strElemType ch_n;
    const strElemType *ch_n_pos;
    memSizeType delta;
    const strElemType *main_mem;
    const strElemType *searched_mem;
    const strElemType *search_start;
    const strElemType *search_end;

  /* strIPos */
    if (unlikely(fromIndex <= 0)) {
      raise_error(RANGE_ERROR);
    } else {
      main_size = mainStri->size;
      searched_size = searched->size;
      if (searched_size != 0 && main_size >= searched_size &&
          (uintType) fromIndex - 1 <= main_size - searched_size) {
        main_size -= (memSizeType) fromIndex - 1;
        if (searched_size >= 2 && main_size >= 1400) {
          return strIPos2(mainStri, searched, fromIndex);
        } else {
          searched_mem = searched->mem;
          ch_n = searched_mem[searched_size - 1];
          ch_n_pos = rsearch_strelem(&searched_mem[searched_size - 2], ch_n, searched_size - 1);
          if (ch_n_pos == NULL) {
            delta = searched_size;
          } else {
            delta = (memSizeType) (&searched_mem[searched_size - 1] - ch_n_pos);
          } /* if */
          main_mem = &mainStri->mem[fromIndex - 1];
          search_start = &main_mem[searched_size - 1];
          search_end = &main_mem[main_size];
          while (search_start < search_end) {
            search_start = search_strelem(search_start, ch_n, search_end);
            if (search_start == NULL) {
              return 0;
            } else {
              if (memcmp(search_start - searched_size + 1, searched_mem,
                  (searched_size - 1) * sizeof(strElemType)) == 0) {
                return ((intType) (search_start - searched_size + 1 - main_mem)) + fromIndex;
              } else {
                search_start += delta;
              } /* if */
            } /* if */
          } /* while */
        } /* if */
      } /* if */
    } /* if */
    return 0;
  } /* strIPos */



/**
 *  Check if stri1 is less than or equal to stri2.
 *  @return TRUE if stri1 is less than or equal to stri2,
 *          FALSE otherwise.
 */
boolType strLe (const const_striType stri1, const const_striType stri2)

  { /* strLe */
    if (stri1->size <= stri2->size) {
      return strelem_memcmp(stri1->mem, stri2->mem, stri1->size) <= 0;
    } else {
      return strelem_memcmp(stri1->mem, stri2->mem, stri2->size) < 0;
    } /* if */
  } /* strLe */



striType strLit (const const_striType stri)

  {
    register strElemType character;
    register memSizeType position;
    memSizeType striSize;
    memSizeType pos;
    char buffer[25];
    memSizeType len;
    striType resized_result;
    striType result;

  /* strLit */
    striSize = stri->size;
    if (unlikely(striSize > (MAX_STRI_LEN - 2) / 12 ||
                 !ALLOC_STRI_SIZE_OK(result, 12 * striSize + 2))) {
      raise_error(MEMORY_ERROR);
      result = NULL;
    } else {
      result->mem[0] = (strElemType) '"';
      pos = 1;
      for (position = 0; position < striSize; position++) {
        character = (strElemType) stri->mem[position];
        if (character < 127) {
          if (character < ' ') {
            result->mem[pos] = (strElemType) '\\';
            if (stri_escape_sequence[character][1] <= '9') {
              /* Numeric escape sequence with one or two digits. */
              if (character <= 9) {
                result->mem[pos + 1] = character + '0';
                result->mem[pos + 2] = (strElemType) ';';
                pos += 3;
              } else {
                result->mem[pos + 1] = (strElemType) stri_escape_sequence[character][1];
                result->mem[pos + 2] = (strElemType) stri_escape_sequence[character][2];
                result->mem[pos + 3] = (strElemType) ';';
                pos += 4;
              } /* if */
            } else {
              /* Character escape sequence. */
              result->mem[pos + 1] = (strElemType) stri_escape_sequence[character][1];
              pos += 2;
            } /* if */
          } else if (character == '\\' || character == '\"') {
            result->mem[pos]     = (strElemType) '\\';
            result->mem[pos + 1] = character;
            pos += 2;
          } else {
            result->mem[pos]     = character;
            pos++;
          } /* if */
        } else if (character <= 160) {
          /* Write characters between 128 and 160 as decimal. */
          /* This code is much faster than sprintf().         */
          result->mem[pos]     = (strElemType) '\\';
          result->mem[pos + 3] = character % 10 + '0';
          character /= 10;
          result->mem[pos + 2] = character % 10 + '0';
          result->mem[pos + 1] = '1';
          result->mem[pos + 4] = (strElemType) ';';
          pos += 5;
        } else if (character >= 256) {
          sprintf(buffer, "\\%lu;", (unsigned long) character);
          len = strlen(buffer);
          memcpy_to_strelem(&result->mem[pos], (const_ustriType) buffer, len);
          pos += len;
        } else {
          result->mem[pos] = character;
          pos++;
        } /* if */
      } /* for */
      result->mem[pos] = (strElemType) '"';
      pos++;
      result->size = pos;
      REALLOC_STRI_SIZE_SMALLER(resized_result, result, 12 * striSize + 2, pos);
      if (unlikely(resized_result == NULL)) {
        FREE_STRI(result, 12 * striSize + 2);
        raise_error(MEMORY_ERROR);
        result = NULL;
      } else {
        result = resized_result;
        COUNT3_STRI(5 * striSize + 2, pos);
      } /* if */
    } /* if */
    return result;
  } /* strLit */



/**
 *  Convert a string to lower case.
 *  The conversion uses the default Unicode case mapping,
 *  where each character is considered in isolation.
 *  Characters without case mapping are left unchanged.
 *  The mapping is independend from the locale. Individual
 *  character case mappings cannot be reversed, because some
 *  characters have multiple characters that map to them.
 *  @return the string converted to lower case.
 */
striType strLow (const const_striType stri)

  {
    memSizeType striSize;
    striType result;

  /* strLow */
    striSize = stri->size;
    if (unlikely(!ALLOC_STRI_SIZE_OK(result, striSize))) {
      raise_error(MEMORY_ERROR);
      return NULL;
    } else {
      result->size = striSize;
      toLower(stri->mem, stri->size, result->mem);
      return result;
    } /* if */
  } /* strLow */



/**
 *  Convert a string to lower case.
 *  StrLowTemp is used by the compiler when 'stri' is temporary
 *  value that can be reused.
 *  @return the string converted to lower case.
 */
striType strLowTemp (const striType stri)

  { /* strLowTemp */
    toLower(stri->mem, stri->size, stri->mem);
    return stri;
  } /* strLowTemp */



/**
 *  Pad a string with spaces at the left side up to pad_size.
 *  @return the string left padded with spaces.
 */
striType strLpad (const const_striType stri, const intType pad_size)

  {
    memSizeType striSize;
    striType result;

  /* strLpad */
    striSize = stri->size;
    if (pad_size > 0 && (uintType) pad_size > striSize) {
      if (unlikely((uintType) pad_size > MAX_STRI_LEN ||
                   !ALLOC_STRI_SIZE_OK(result, (memSizeType) pad_size))) {
        raise_error(MEMORY_ERROR);
        result = NULL;
      } else {
        result->size = (memSizeType) pad_size;
#if LPAD_WITH_MEMSET_TO_STRELEM
        memset_to_strelem(result->mem, ' ', (memSizeType) pad_size - striSize);
#else
        {
          strElemType *elem = result->mem;
          memSizeType idx = (memSizeType) pad_size - striSize - 1;

          do {
            elem[idx] = (strElemType) ' ';
          } while (idx-- != 0);
        }
#endif
        memcpy(&result->mem[(memSizeType) pad_size - striSize], stri->mem,
            striSize * sizeof(strElemType));
      } /* if */
    } else {
      if (unlikely(!ALLOC_STRI_SIZE_OK(result, striSize))) {
        raise_error(MEMORY_ERROR);
      } else {
        result->size = striSize;
        memcpy(result->mem, stri->mem, striSize * sizeof(strElemType));
      } /* if */
    } /* if */
    return result;
  } /* strLpad */



/**
 *  Pad a string with spaces at the left side up to pad_size.
 *  StrLpadTemp is used by the compiler when 'stri' is temporary
 *  value that can be reused.
 *  @return the string left padded with spaces.
 */
striType strLpadTemp (const striType stri, const intType pad_size)

  {
    memSizeType striSize;
    striType result;

  /* strLpadTemp */
    striSize = stri->size;
    if (pad_size > 0 && (uintType) pad_size > striSize) {
      if (unlikely((uintType) pad_size > MAX_STRI_LEN ||
                   !ALLOC_STRI_SIZE_OK(result, (memSizeType) pad_size))) {
        raise_error(MEMORY_ERROR);
        result = NULL;
      } else {
        result->size = (memSizeType) pad_size;
#if LPAD_WITH_MEMSET_TO_STRELEM
        memset_to_strelem(result->mem, ' ', (memSizeType) pad_size - striSize);
#else
        {
          strElemType *elem = result->mem;
          memSizeType idx = (memSizeType) pad_size - striSize - 1;

          do {
            elem[idx] = (strElemType) ' ';
          } while (idx-- != 0);
        }
#endif
        memcpy(&result->mem[(memSizeType) pad_size - striSize], stri->mem,
            striSize * sizeof(strElemType));
        FREE_STRI(stri, striSize);
      } /* if */
    } else {
      result = stri;
    } /* if */
    return result;
  } /* strLpadTemp */



/**
 *  Pad a string with zeroes at the left side up to pad_size.
 *  @return the string left padded with zeroes.
 */
striType strLpad0 (const const_striType stri, const intType pad_size)

  {
    memSizeType striSize;
    const strElemType *sourceElem;
    strElemType *destElem;
    memSizeType len;
    striType result;

  /* strLpad0 */
    striSize = stri->size;
    if (pad_size > 0 && (uintType) pad_size > striSize) {
      if (unlikely((uintType) pad_size > MAX_STRI_LEN ||
                   !ALLOC_STRI_SIZE_OK(result, (memSizeType) pad_size))) {
        raise_error(MEMORY_ERROR);
        result = NULL;
      } else {
        result->size = (memSizeType) pad_size;
        sourceElem = stri->mem;
        destElem = result->mem;
        len = (memSizeType) pad_size - striSize;
        if (striSize != 0 && (sourceElem[0] == '-' || sourceElem[0] == '+')) {
          *destElem++ = sourceElem[0];
          sourceElem++;
          striSize--;
        } /* if */
        while (len--) {
          *destElem++ = (strElemType) '0';
        } /* while */
        memcpy(destElem, sourceElem, striSize * sizeof(strElemType));
      } /* if */
    } else {
      if (unlikely(!ALLOC_STRI_SIZE_OK(result, striSize))) {
        raise_error(MEMORY_ERROR);
      } else {
        result->size = striSize;
        memcpy(result->mem, stri->mem, striSize * sizeof(strElemType));
      } /* if */
    } /* if */
    return result;
  } /* strLpad0 */



/**
 *  Pad a string with zeroes at the left side up to pad_size.
 *  StrLpad0Temp is used by the compiler when 'stri' is temporary
 *  value that can be reused.
 *  @return the string left padded with zeroes.
 */
striType strLpad0Temp (const striType stri, const intType pad_size)

  {
    memSizeType striSize;
    const strElemType *sourceElem;
    strElemType *destElem;
    memSizeType len;
    striType result;

  /* strLpad0Temp */
    striSize = stri->size;
    if (pad_size > 0 && (uintType) pad_size > striSize) {
      if (unlikely((uintType) pad_size > MAX_STRI_LEN ||
                   !ALLOC_STRI_SIZE_OK(result, (memSizeType) pad_size))) {
        raise_error(MEMORY_ERROR);
        result = NULL;
      } else {
        result->size = (memSizeType) pad_size;
        sourceElem = stri->mem;
        destElem = result->mem;
        len = (memSizeType) pad_size - striSize;
        if (striSize != 0 && (sourceElem[0] == '-' || sourceElem[0] == '+')) {
          *destElem++ = sourceElem[0];
          sourceElem++;
          striSize--;
        } /* if */
        while (len--) {
          *destElem++ = (strElemType) '0';
        } /* while */
        memcpy(destElem, sourceElem, striSize * sizeof(strElemType));
        FREE_STRI(stri, striSize);
      } /* if */
    } else {
      result = stri;
    } /* if */
    return result;
  } /* strLpad0Temp */



/**
 *  Check if stri1 is less than stri2.
 *  @return TRUE if stri1 is less than stri2,
 *          FALSE otherwise.
 */
boolType strLt (const const_striType stri1, const const_striType stri2)

  { /* strLt */
    if (stri1->size < stri2->size) {
      return strelem_memcmp(stri1->mem, stri2->mem, stri1->size) <= 0;
    } else {
      return strelem_memcmp(stri1->mem, stri2->mem, stri2->size) < 0;
    } /* if */
  } /* strLt */



/**
 *  Return string with leading whitespace omitted.
 *  All characters less than or equal to ' ' (space) count as whitespace.
 *  @return string with leading whitespace omitted.
 */
striType strLtrim (const const_striType stri)

  {
    memSizeType start;
    memSizeType striSize;
    striType result;

  /* strLtrim */
    start = 0;
    striSize = stri->size;
    if (striSize >= 1) {
      while (start < striSize && stri->mem[start] <= ' ') {
        start++;
      } /* while */
      striSize -= start;
    } /* if */
    if (unlikely(!ALLOC_STRI_SIZE_OK(result, striSize))) {
      raise_error(MEMORY_ERROR);
      return NULL;
    } else {
      result->size = striSize;
      memcpy(result->mem, &stri->mem[start], striSize * sizeof(strElemType));
      return result;
    } /* if */
  } /* strLtrim */



striType strMult (const const_striType stri, const intType factor)

  {
    memSizeType len;
    memSizeType count;
    memSizeType powerOfTwo;
    strElemType ch;
    memSizeType result_size;
    striType result;

  /* strMult */
#ifdef TRACE_STR_RTL
    printf("strMult(stri->size=" FMT_U_MEM ", " FMT_D ")\n",
           stri->size, factor);
#endif
    if (unlikely(factor < 0)) {
      raise_error(RANGE_ERROR);
      result = NULL;
    } else {
      len = stri->size;
      if (unlikely(len == 0)) {
        if (unlikely(!ALLOC_STRI_SIZE_OK(result, 0))) {
          raise_error(MEMORY_ERROR);
        } else {
          result->size = 0;
        } /* if */
      } else if (unlikely((uintType) factor > MAX_STRI_LEN / len)) {
        raise_error(MEMORY_ERROR);
        result = NULL;
      } else {
        count = (memSizeType) factor;
        result_size = count * len;
        if (unlikely(!ALLOC_STRI_SIZE_OK(result, result_size))) {
          raise_error(MEMORY_ERROR);
        } else {
          result->size = result_size;
          if (len == 1) {
            ch = stri->mem[0];
            memset_to_strelem(result->mem, ch, count);
          } else if (count != 0) {
            /* Use binary method for string multiplication: */
            memcpy(result->mem, stri->mem, len * sizeof(strElemType));
            powerOfTwo = 1;
            while (powerOfTwo << 1 < count) {
              memcpy(&result->mem[powerOfTwo * len], result->mem,
                     powerOfTwo * len * sizeof(strElemType));
              powerOfTwo <<= 1;
            } /* while */
            memcpy(&result->mem[powerOfTwo * len], result->mem,
                   (count - powerOfTwo) * len * sizeof(strElemType));
          } /* if */
        } /* if */
      } /* if */
    } /* if */
    return result;
  } /* strMult */



/**
 *  Determine leftmost position of string 'searched' in 'mainStri'.
 *  When the string is found the position of its first character
 *  is the result. The first character in a string has the position 1.
 *  This function uses a modified Boyer–Moore string search algorithm.
 *  @return the position of 'searched' or 0 when 'mainStri'
 *          does not contain 'searched'.
 */
static intType strPos2 (const const_striType mainStri, const const_striType searched)

  {
    memSizeType main_size;
    memSizeType searched_size;
    strElemType ch_n;
    const strElemType *ch_n_pos;
    memSizeType delta;
    memSizeType charDelta[CHAR_DELTA_BEYOND + 1];
    memSizeType pos;
    const strElemType *main_mem;
    const strElemType *searched_mem;
    const strElemType *search_start;
    const strElemType *search_end;

  /* strPos2 */
    main_size = mainStri->size;
    searched_size = searched->size;
    for (ch_n = 0; ch_n <= CHAR_DELTA_BEYOND; ch_n++) {
      charDelta[ch_n] = searched_size;
    } /* for */
    searched_mem = searched->mem;
    for (pos = 0; pos < searched_size - 1; pos++) {
      ch_n = searched_mem[pos];
      if (ch_n < CHAR_DELTA_BEYOND) {
        charDelta[ch_n] = searched_size - pos - 1;
      } else {
        charDelta[CHAR_DELTA_BEYOND] = searched_size - pos - 1;
      } /* if */
    } /* for */
    ch_n = searched_mem[searched_size - 1];
    ch_n_pos = rsearch_strelem(&searched_mem[searched_size - 2], ch_n, searched_size - 1);
    if (ch_n_pos == NULL) {
      delta = searched_size;
    } else {
      delta = (memSizeType) (&searched_mem[searched_size - 1] - ch_n_pos);
    } /* if */
    main_mem = mainStri->mem;
    search_start = &main_mem[searched_size - 1];
    search_end = &main_mem[main_size];
    while (search_start < search_end) {
      search_start = search_strelem2(search_start, ch_n, search_end, charDelta);
      if (search_start == NULL) {
        return 0;
      } else {
        if (memcmp(search_start - searched_size + 1, searched_mem,
            (searched_size - 1) * sizeof(strElemType)) == 0) {
          return ((intType) (search_start - searched_size + 1 - main_mem)) + 1;
        } else {
          search_start += delta;
        } /* if */
      } /* if */
    } /* while */
    return 0;
  } /* strPos2 */



/**
 *  Determine leftmost position of string 'searched' in 'mainStri'.
 *  When the string is found the position of its first character
 *  is the result. The first character in a string has the position 1.
 *  This function calls strPos2 when 'mainStri' is long.
 *  @return the position of 'searched' or 0 when 'mainStri'
 *          does not contain 'searched'.
 */
intType strPos (const const_striType mainStri, const const_striType searched)

  {
    memSizeType main_size;
    memSizeType searched_size;
    strElemType ch_n;
    const strElemType *ch_n_pos;
    memSizeType delta;
    const strElemType *main_mem;
    const strElemType *searched_mem;
    const strElemType *search_start;
    const strElemType *search_end;

  /* strPos */
    main_size = mainStri->size;
    searched_size = searched->size;
    if (searched_size != 0 && main_size >= searched_size) {
      if (searched_size >= 2 && main_size >= 1400) {
        return strPos2(mainStri, searched);
      } else {
        searched_mem = searched->mem;
        ch_n = searched_mem[searched_size - 1];
        ch_n_pos = rsearch_strelem(&searched_mem[searched_size - 2], ch_n, searched_size - 1);
        if (ch_n_pos == NULL) {
          delta = searched_size;
        } else {
          delta = (memSizeType) (&searched_mem[searched_size - 1] - ch_n_pos);
        } /* if */
        main_mem = mainStri->mem;
        search_start = &main_mem[searched_size - 1];
        search_end = &main_mem[main_size];
        while (search_start < search_end) {
          search_start = search_strelem(search_start, ch_n, search_end);
          if (search_start == NULL) {
            return 0;
          } else {
            if (memcmp(search_start - searched_size + 1, searched_mem,
                (searched_size - 1) * sizeof(strElemType)) == 0) {
              return ((intType) (search_start - searched_size + 1 - main_mem)) + 1;
            } else {
              search_start += delta;
            } /* if */
          } /* if */
        } /* while */
      } /* if */
    } /* if */
    return 0;
  } /* strPos */



/**
 *  Append the char 'extension' to 'destination'.
 *  @exception MEMORY_ERROR Not enough memory for the concatenated
 *             string.
 */
void strPush (striType *const destination, const charType extension)

  {
    memSizeType new_size;
    striType stri_dest;

  /* strPush */
    stri_dest = *destination;
    new_size = stri_dest->size + 1;
#ifdef WITH_STRI_CAPACITY
    if (new_size > stri_dest->capacity) {
      stri_dest = growStri(stri_dest, new_size);
      if (unlikely(stri_dest == NULL)) {
        raise_error(MEMORY_ERROR);
        return;
      } else {
        *destination = stri_dest;
      } /* if */
    } /* if */
    COUNT3_STRI(stri_dest->size, new_size);
    stri_dest->mem[stri_dest->size] = extension;
    stri_dest->size = new_size;
#else
    GROW_STRI(stri_dest, stri_dest, stri_dest->size, new_size);
    if (unlikely(stri_dest == NULL)) {
      raise_error(MEMORY_ERROR);
    } else {
      COUNT3_STRI(stri_dest->size, new_size);
      stri_dest->mem[stri_dest->size] = extension;
      stri_dest->size = new_size;
      *destination = stri_dest;
    } /* if */
#endif
  } /* strPush */



#ifdef ALLOW_STRITYPE_SLICES
/**
 *  Get a substring from a start position to a stop position.
 *  The first character in a string has the position 1.
 *  This function is used by the compiler to avoid copiing string data.
 *  The 'slice' is initialized to refer to the range of 'stri'
 */
void strRangeSlice (const const_striType stri, intType start, intType stop, striType slice)

  {
    memSizeType striSize;

  /* strRangeSlice */
#ifdef TRACE_STR_RTL
    printf("strRangeSlice(");
    prot_stri(stri);
    printf(", " FMT_D ", " FMT_D ")", start, stop);
    prot_flush();
#endif
    striSize = stri->size;
    if (unlikely(start < 1)) {
      start = 1;
    } /* if */
    SET_SLICE_CAPACITY(slice, 0);
    if (stop >= start && (uintType) start <= striSize) {
      slice->mem = &stri->mem[start - 1];
      if ((uintType) stop > striSize) {
        slice->size = striSize - (memSizeType) start + 1;
      } else {
        slice->size = (memSizeType) stop - (memSizeType) start + 1;
      } /* if */
    } else {
      slice->mem = NULL;
      slice->size = 0;
    } /* if */
#ifdef TRACE_STR_RTL
    printf(" --> ");
    prot_stri(slice);
    printf("\n");
#endif
  } /* strRangeSlice */

#endif



/**
 *  Get a substring from a start position to a stop position.
 *  The first character in a string has the position 1.
 *  @return the substring from position start to stop.
 *  @exception MEMORY_ERROR Not enough memory to represent the result.
 */
striType strRange (const const_striType stri, intType start, intType stop)

  {
    memSizeType striSize;
    memSizeType result_size;
    striType result;

  /* strRange */
    striSize = stri->size;
    if (unlikely(start < 1)) {
      start = 1;
    } /* if */
    if (stop >= start && (uintType) start <= striSize) {
      if ((uintType) stop > striSize) {
        result_size = striSize - (memSizeType) start + 1;
      } else {
        result_size = (memSizeType) stop - (memSizeType) start + 1;
      } /* if */
      if (unlikely(!ALLOC_STRI_SIZE_OK(result, result_size))) {
        raise_error(MEMORY_ERROR);
        return NULL;
      } /* if */
      /* Reversing the order of the following two statements    */
      /* causes an "Internal Compiler Error" with MSC 6.0       */
      /* when using the -Ozacegilt optimisation option in the   */
      /* large memory model (-AL). Note that the order of the   */
      /* two statements make no difference to the logic of the  */
      /* program.                                               */
      memcpy(result->mem, &stri->mem[start - 1],
          result_size * sizeof(strElemType));
      result->size = result_size;
    } else {
      if (unlikely(!ALLOC_STRI_SIZE_OK(result, (memSizeType) 0))) {
        raise_error(MEMORY_ERROR);
        return NULL;
      } /* if */
      result->size = 0;
    } /* if */
    return result;
  } /* strRange */



/**
 *  Search char 'searched' in 'mainStri' at or before 'fromIndex'.
 *  The search starts at 'fromIndex' and proceeds to the left.
 *  The first character in a string has the position 1.
 *  @return the position of 'searched' or 0 when 'mainStri'
 *          does not contain 'searched' at or before 'fromIndex'.
 *  @exception RANGE_ERROR 'fromIndex' > length(stri) holds.
 */
intType strRChIPos (const const_striType mainStri, const charType searched,
    const intType fromIndex)

  {
    const strElemType *main_mem;
    const strElemType *found_pos;

  /* strRChIPos */
    if (likely(fromIndex >= 1)) {
      if (unlikely((uintType) fromIndex > mainStri->size)) {
        raise_error(RANGE_ERROR);
      } else {
        if (mainStri->size >= 1) {
          main_mem = mainStri->mem;
          found_pos = rsearch_strelem(&main_mem[fromIndex - 1], searched,
              (memSizeType) fromIndex);
          if (found_pos != NULL) {
            return ((intType) (found_pos - main_mem)) + 1;
          } /* if */
        } /* if */
      } /* if */
    } /* if */
    return 0;
  } /* strRChIPos */



/**
 *  Determine rightmost position of char 'searched' in 'mainStri'.
 *  The first character in a string has the position 1.
 *  @return the position of 'searched' or 0 when 'mainStri'
 *          does not contain 'searched'.
 */
intType strRChPos (const const_striType mainStri, const charType searched)

  {
    const strElemType *main_mem;
    const strElemType *found_pos;

  /* strRChPos */
    if (mainStri->size >= 1) {
      main_mem = mainStri->mem;
      found_pos = rsearch_strelem(&main_mem[mainStri->size - 1], searched,
          mainStri->size);
      if (found_pos != NULL) {
        return ((intType) (found_pos - main_mem)) + 1;
      } /* if */
    } /* if */
    return 0;
  } /* strRChPos */



/**
 *  Replace all occurrences of 'searched' in 'mainStri' by 'replacement'.
 *  This function uses a modified Boyer–Moore string search algorithm.
 *  @return the result of the replacement.
 */
static memSizeType strRepl2 (const const_striType mainStri, const const_striType searched,
    const const_striType replacement, const striType result)

  {
    memSizeType main_size;
    memSizeType searched_size;
    strElemType ch_n;
    const strElemType *ch_n_pos;
    memSizeType delta;
    memSizeType charDelta[CHAR_DELTA_BEYOND + 1];
    memSizeType pos;
    const strElemType *main_mem;
    const strElemType *searched_mem;
    const strElemType *search_start;
    const strElemType *search_end;
    const strElemType *copy_start;
    strElemType *result_end;
    memSizeType result_size;

  /* strRepl2 */
    main_size = mainStri->size;
    searched_size = searched->size;
    for (ch_n = 0; ch_n <= CHAR_DELTA_BEYOND; ch_n++) {
      charDelta[ch_n] = searched_size;
    } /* for */
    searched_mem = searched->mem;
    for (pos = 0; pos < searched_size - 1; pos++) {
      ch_n = searched_mem[pos];
      if (ch_n < CHAR_DELTA_BEYOND) {
        charDelta[ch_n] = searched_size - pos - 1;
      } else {
        charDelta[CHAR_DELTA_BEYOND] = searched_size - pos - 1;
      } /* if */
    } /* for */
    ch_n = searched_mem[searched_size - 1];
    ch_n_pos = rsearch_strelem(&searched_mem[searched_size - 2], ch_n, searched_size - 1);
    if (ch_n_pos == NULL) {
      delta = searched_size;
    } else {
      delta = (memSizeType) (&searched_mem[searched_size - 1] - ch_n_pos);
    } /* if */
    main_mem = mainStri->mem;
    search_start = &main_mem[searched_size - 1];
    search_end = &main_mem[main_size];
    copy_start = mainStri->mem;
    result_end = result->mem;
    while (search_start != NULL && search_start < search_end) {
      search_start = search_strelem2(search_start, ch_n, search_end, charDelta);
      if (search_start != NULL) {
        if (memcmp(search_start - searched_size + 1, searched_mem,
            (searched_size - 1) * sizeof(strElemType)) == 0) {
          memcpy(result_end, copy_start,
              (memSizeType) (search_start - searched_size + 1 - copy_start) * sizeof(strElemType));
          result_end += search_start - searched_size + 1 - copy_start;
          memcpy(result_end, replacement->mem,
              replacement->size * sizeof(strElemType));
          result_end += replacement->size;
          copy_start = search_start + 1;
          search_start += searched_size;
        } else {
          search_start += delta;
        } /* if */
      } /* if */
    } /* while */
    memcpy(result_end, copy_start,
        (memSizeType) (&mainStri->mem[main_size] - copy_start) * sizeof(strElemType));
    result_end += &mainStri->mem[main_size] - copy_start;
    result_size = (memSizeType) (result_end - result->mem);
    return result_size;
  } /* strRepl2 */



/**
 *  Replace all occurrences of 'searched' in 'mainStri' by 'replacement'.
 *  This function calls strRepl2 when 'mainStri' is long.
 *  @return the result of the replacement.
 */
striType strRepl (const const_striType mainStri,
    const const_striType searched, const const_striType replacement)

  {
    memSizeType main_size;
    memSizeType searched_size;
    memSizeType guessed_result_size;
    memSizeType result_size;
    strElemType ch_1;
    const strElemType *main_mem;
    const strElemType *searched_mem;
    const strElemType *search_start;
    const strElemType *search_end;
    const strElemType *copy_start;
    strElemType *result_end;
    striType resized_result;
    striType result;

  /* strRepl */
    main_size = mainStri->size;
    searched_size = searched->size;
    /* printf("main_size=" FMT_U_MEM ", searched_size=" FMT_U_MEM ", replacement->size=" FMT_U_MEM "\n",
       main_size, searched_size, replacement->size); */
    if (searched_size != 0 && replacement->size > searched_size) {
      if (unlikely(main_size / searched_size + 1 > MAX_STRI_LEN / replacement->size)) {
        raise_error(MEMORY_ERROR);
        return NULL;
      } else {
        guessed_result_size = (main_size / searched_size + 1) * replacement->size;
      } /* if */
    } else {
      guessed_result_size = main_size;
    } /* if */
    if (unlikely(!ALLOC_STRI_SIZE_OK(result, guessed_result_size))) {
      raise_error(MEMORY_ERROR);
    } else {
      if (searched_size >= 2 && main_size >= 1400) {
        result_size = strRepl2(mainStri, searched, replacement, result);
      } else {
        copy_start = mainStri->mem;
        result_end = result->mem;
        if (searched_size != 0 && searched_size <= main_size) {
          searched_mem = searched->mem;
          ch_1 = searched_mem[0];
          main_mem = mainStri->mem;
          search_start = main_mem;
          search_end = &main_mem[main_size - searched_size + 1];
          while (search_start < search_end &&
              (search_start = search_strelem(search_start, ch_1, search_end)) != NULL) {
            if (memcmp(search_start, searched_mem,
                searched_size * sizeof(strElemType)) == 0) {
              memcpy(result_end, copy_start,
                  (memSizeType) (search_start - copy_start) * sizeof(strElemType));
              result_end += search_start - copy_start;
              memcpy(result_end, replacement->mem,
                  replacement->size * sizeof(strElemType));
              result_end += replacement->size;
              search_start += searched_size;
              copy_start = search_start;
            } else {
              search_start++;
            } /* if */
          } /* if */
        } /* if */
        memcpy(result_end, copy_start,
            (memSizeType) (&mainStri->mem[main_size] - copy_start) * sizeof(strElemType));
        result_end += &mainStri->mem[main_size] - copy_start;
        result_size = (memSizeType) (result_end - result->mem);
      } /* if */
      /* printf("result=%lu, guessed_result_size=%ld, result_size=%ld\n",
         result, guessed_result_size, result_size); */
      REALLOC_STRI_SIZE_SMALLER(resized_result, result, guessed_result_size, result_size);
      if (unlikely(resized_result == NULL)) {
        FREE_STRI(result, guessed_result_size);
        raise_error(MEMORY_ERROR);
        result = NULL;
      } else {
        result = resized_result;
        COUNT3_STRI(guessed_result_size, result_size);
        result->size = result_size;
      } /* if */
    } /* if */
    return result;
  } /* strRepl */



/**
 *  Search string 'searched' in 'mainStri' at or before 'fromIndex'.
 *  The search starts at 'fromIndex' and proceeds to the left.
 *  The first character in a string has the position 1.
 *  This function uses a modified Boyer–Moore string search algorithm.
 *  @return the position of 'searched' or 0 when 'mainStri'
 *          does not contain 'searched' at or before 'fromIndex'.
 */
static intType strRIPos2 (const const_striType mainStri, const const_striType searched,
    const intType fromIndex)

  {
    memSizeType main_size;
    memSizeType searched_size;
    strElemType ch_1;
    const strElemType *ch_1_pos;
    memSizeType delta;
    memSizeType charDelta[CHAR_DELTA_BEYOND + 1];
    memSizeType pos;
    const strElemType *main_mem;
    const strElemType *searched_mem;
    const strElemType *search_start;
    const strElemType *search_end;

  /* strRIPos2 */
    main_size = mainStri->size;
    searched_size = searched->size;
    for (ch_1 = 0; ch_1 <= CHAR_DELTA_BEYOND; ch_1++) {
      charDelta[ch_1] = searched_size;
    } /* for */
    searched_mem = searched->mem;
    for (pos = searched_size - 1; pos > 0; pos--) {
      ch_1 = searched_mem[pos];
      if (ch_1 < CHAR_DELTA_BEYOND) {
        charDelta[ch_1] = pos;
      } else {
        charDelta[CHAR_DELTA_BEYOND] = pos;
      } /* if */
    } /* for */
    ch_1 = searched_mem[0];
    ch_1_pos = search_strelem(&searched_mem[1], ch_1, &searched_mem[searched_size]);
    if (ch_1_pos == NULL) {
      delta = searched_size;
    } else {
      delta = (memSizeType) (ch_1_pos - &searched_mem[0]);
    } /* if */
    main_mem = mainStri->mem;
    if ((uintType) fromIndex - 1 <= main_size - searched_size) {
      search_start = &main_mem[fromIndex - 1];
    } else {
      search_start = &main_mem[main_size - searched_size];
    } /* if */
    search_end = &main_mem[-1];
    while (search_start > search_end) {
      search_start = rsearch_strelem2(search_start, ch_1, search_end, charDelta);
      if (search_start == NULL) {
        return 0;
      } else {
        if (memcmp(search_start + 1, &searched_mem[1],
            (searched_size - 1) * sizeof(strElemType)) == 0) {
          return ((intType) (search_start - main_mem)) + 1;
        } else {
          search_start -= delta;
        } /* if */
      } /* if */
    } /* while */
    return 0;
  } /* strRIPos2 */



/**
 *  Search string 'searched' in 'mainStri' at or before 'fromIndex'.
 *  The search starts at 'fromIndex' and proceeds to the left.
 *  The first character in a string has the position 1.
 *  This function calls strRIPos2 when 'mainStri' is long.
 *  @return the position of 'searched' or 0 when 'mainStri'
 *          does not contain 'searched' at or before 'fromIndex'.
 *  @exception RANGE_ERROR 'fromIndex' > length(stri) holds.
 */
intType strRIPos (const const_striType mainStri, const const_striType searched,
    const intType fromIndex)

  {
    memSizeType main_size;
    memSizeType searched_size;
    strElemType ch_1;
    const strElemType *searched_mem;
    const strElemType *main_mem;
    const strElemType *search_start;
    const strElemType *search_end;

  /* strRIPos */
    if (likely(fromIndex >= 1)) {
      if (unlikely((uintType) fromIndex > mainStri->size)) {
        raise_error(RANGE_ERROR);
      } else {
        main_size = mainStri->size;
        searched_size = searched->size;
        if (searched_size != 0 && main_size >= searched_size) {
          if (searched_size >= 2 && main_size >= 1400) {
            return strRIPos2(mainStri, searched, fromIndex);
          } else {
            searched_mem = searched->mem;
            ch_1 = searched_mem[0];
            main_mem = mainStri->mem;
            if ((uintType) fromIndex - 1 <= main_size - searched_size) {
              search_start = &main_mem[fromIndex - 1];
            } else {
              search_start = &main_mem[main_size - searched_size];
            } /* if */
            search_end = &main_mem[-1];
            while ((search_start = rsearch_strelem(search_start,
                ch_1, (memSizeType) (search_start - search_end))) != NULL) {
              if (memcmp(search_start, searched_mem,
                  searched_size * sizeof(strElemType)) == 0) {
                return ((intType) (search_start - main_mem)) + 1;
              } else {
                search_start--;
              } /* if */
            } /* if */
          } /* if */
        } /* if */
      } /* if */
    } /* if */
    return 0;
  } /* strRIPos */



/**
 *  Pad a string with spaces at the right side up to pad_size.
 *  @return the string right padded with spaces.
 */
striType strRpad (const const_striType stri, const intType pad_size)

  {
    memSizeType striSize;
    striType result;

  /* strRpad */
    striSize = stri->size;
    if (pad_size > 0 && (uintType) pad_size > striSize) {
      if (unlikely((uintType) pad_size > MAX_STRI_LEN ||
                   !ALLOC_STRI_SIZE_OK(result, (memSizeType) pad_size))) {
        raise_error(MEMORY_ERROR);
        result = NULL;
      } else {
        result->size = (memSizeType) pad_size;
        memcpy(result->mem, stri->mem, striSize * sizeof(strElemType));
        {
          strElemType *elem = &result->mem[striSize];
          memSizeType len = (memSizeType) pad_size - striSize;

          while (len--) {
            *elem++ = (strElemType) ' ';
          } /* while */
        }
      } /* if */
    } else {
      if (unlikely(!ALLOC_STRI_SIZE_OK(result, striSize))) {
        raise_error(MEMORY_ERROR);
        return NULL;
      } /* if */
      result->size = striSize;
      memcpy(result->mem, stri->mem, striSize * sizeof(strElemType));
    } /* if */
    return result;
  } /* strRpad */



/**
 *  Determine rightmost position of string 'searched' in 'mainStri'.
 *  When the string is found the position of its first character
 *  is the result. The first character in a string has the position 1.
 *  This function uses a modified Boyer–Moore string search algorithm.
 *  @return the position of 'searched' or 0 when 'mainStri'
 *          does not contain 'searched'.
 */
static intType strRPos2 (const const_striType mainStri, const const_striType searched)

  {
    memSizeType main_size;
    memSizeType searched_size;
    strElemType ch_1;
    const strElemType *ch_1_pos;
    memSizeType delta;
    memSizeType charDelta[CHAR_DELTA_BEYOND + 1];
    memSizeType pos;
    const strElemType *main_mem;
    const strElemType *searched_mem;
    const strElemType *search_start;
    const strElemType *search_end;

  /* strRPos2 */
    main_size = mainStri->size;
    searched_size = searched->size;
    for (ch_1 = 0; ch_1 <= CHAR_DELTA_BEYOND; ch_1++) {
      charDelta[ch_1] = searched_size;
    } /* for */
    searched_mem = searched->mem;
    for (pos = searched_size - 1; pos > 0; pos--) {
      ch_1 = searched_mem[pos];
      if (ch_1 < CHAR_DELTA_BEYOND) {
        charDelta[ch_1] = pos;
      } else {
        charDelta[CHAR_DELTA_BEYOND] = pos;
      } /* if */
    } /* for */
    ch_1 = searched_mem[0];
    ch_1_pos = search_strelem(&searched_mem[1], ch_1, &searched_mem[searched_size]);
    if (ch_1_pos == NULL) {
      delta = searched_size;
    } else {
      delta = (memSizeType) (ch_1_pos - &searched_mem[0]);
    } /* if */
    main_mem = mainStri->mem;
    search_start = &main_mem[main_size - searched_size];
    search_end = &main_mem[-1];
    while (search_start > search_end) {
      search_start = rsearch_strelem2(search_start, ch_1, search_end, charDelta);
      if (search_start == NULL) {
        return 0;
      } else {
        if (memcmp(search_start + 1, &searched_mem[1],
            (searched_size - 1) * sizeof(strElemType)) == 0) {
          return ((intType) (search_start - main_mem)) + 1;
        } else {
          search_start -= delta;
        } /* if */
      } /* if */
    } /* while */
    return 0;
  } /* strRPos2 */



/**
 *  Determine rightmost position of string 'searched' in 'mainStri'.
 *  When the string is found the position of its first character
 *  is the result. The first character in a string has the position 1.
 *  This function calls strRPos2 when 'mainStri' is long.
 *  @return the position of 'searched' or 0 when 'mainStri'
 *          does not contain 'searched'.
 */
intType strRPos (const const_striType mainStri, const const_striType searched)

  {
    memSizeType main_size;
    memSizeType searched_size;
    strElemType ch_1;
    const strElemType *main_mem;
    const strElemType *searched_mem;
    const strElemType *search_start;
    const strElemType *search_end;

  /* strRPos */
    main_size = mainStri->size;
    searched_size = searched->size;
    if (searched_size != 0 && searched_size <= main_size) {
      if (searched_size >= 2 && main_size >= 1400) {
        return strRPos2(mainStri, searched);
      } else {
        searched_mem = searched->mem;
        ch_1 = searched_mem[0];
        main_mem = mainStri->mem;
        search_start = &main_mem[main_size - searched_size];
        search_end = &main_mem[-1];
        while ((search_start = rsearch_strelem(search_start,
            ch_1, (memSizeType) (search_start - search_end))) != NULL) {
          if (memcmp(search_start, searched_mem,
              searched_size * sizeof(strElemType)) == 0) {
            return ((intType) (search_start - main_mem)) + 1;
          } else {
            search_start--;
          } /* if */
        } /* if */
      } /* if */
    } /* if */
    return 0;
  } /* strRPos */



/**
 *  Return string with trailing whitespace omitted.
 *  All characters less than or equal to ' ' (space) count as whitespace.
 *  @return string with trailing whitespace omitted.
 */
striType strRtrim (const const_striType stri)

  {
    memSizeType striSize;
    striType result;

  /* strRtrim */
    striSize = stri->size;
    while (striSize > 0 && stri->mem[striSize - 1] <= ' ') {
      striSize--;
    } /* while */
    if (unlikely(!ALLOC_STRI_SIZE_OK(result, striSize))) {
      raise_error(MEMORY_ERROR);
      return NULL;
    } else {
      result->size = striSize;
      memcpy(result->mem, stri->mem, striSize * sizeof(strElemType));
      return result;
    } /* if */
  } /* strRtrim */



#ifdef OUT_OF_ORDER
arrayType strSplit (const const_striType mainStri, charType delimiter)

  {
    arrayType result_array;
    intType used_max_position;
    errInfoType err_info = OKAY_NO_ERROR;

    memSizeType main_size;
    const strElemType *main_mem;
    const strElemType *search_start;
    const strElemType *search_end;
    const strElemType *found_pos;
    striType dest;

  /* strSplit */
    if (unlikely(!ALLOC_ARRAY(result_array, 256))) {
      raise_error(MEMORY_ERROR);
      return NULL;
    } else {
      result_array->min_position = 1;
      result_array->max_position = 256;
      used_max_position = 0;
      main_size = mainStri->size;
      if (main_size >= 1) {
        main_mem = mainStri->mem;
        search_start = main_mem;
        search_end = &main_mem[main_size];
        while ((found_pos = search_strelem(search_start, delimiter, search_end)) != NULL) {
          add_stri_to_array(search_start, found_pos - search_start,
              result_array, &used_max_position, &err_info);
          search_start = found_pos + 1
        } /* while */
        add_stri_to_array(search_start, search_end - search_start,
            result_array, &used_max_position, &err_info);
      } /* if */

      return result_array;
    } /* if */
  } /* strSplit */



arrayType strSplit (const const_striType stri, const const_striType delimiter)

  {
    memSizeType main_size;
    memSizeType delimiter_size;
    strElemType ch_1;
    const strElemType *main_mem;
    const strElemType *delimiter_mem;
    const strElemType *search_start;
    const strElemType *search_end;
    const strElemType *found_pos;
    errInfoType err_info = OKAY_NO_ERROR;

  /* strSplit */
    main_size = mainStri->size;
    delimiter_size = delimiter->size;
    if (delimiter->size == 1) {
      if (delimiter_size <= main_size) {
        delimiter_mem = delimiter->mem;
        ch_1 = delimiter_mem[0];
        main_mem = mainStri->mem;
        search_start = main_mem;
        search_end = &main_mem[main_size - delimiter_size + 1];
        while ((found_pos = search_strelem(search_start, ch_1, search_end)) != NULL) {
          memcpy(dest, search_start, (memSizeType) (found_pos - search_start));
          search_start = found_pos + 1
        } /* while */
      } /* if */
    } else {
      if (delimiter_size != 0 && delimiter_size <= main_size) {
        delimiter_mem = delimiter->mem;
        ch_1 = delimiter_mem[0];
        main_mem = mainStri->mem;
        search_start = main_mem;
        search_end = &main_mem[main_size - delimiter_size + 1];
        while ((found_pos = search_strelem(search_start, ch_1, search_end)) != NULL) {
          if (memcmp(search_start, delimiter_mem,
              delimiter_size * sizeof(strElemType)) == 0) {

            search_start = found_pos + delimiter_size;
          } else {
            search_start++;
          } /* if */
        } /* while */
      } /* if */
    } /* if */
  } /* strSplit */
#endif



rtlArrayType strSplit (const const_striType mainStri,
    const const_striType delimiter)

  {
    memSizeType delimiter_size;
    const strElemType *delimiter_mem;
    strElemType ch_1;
    intType used_max_position;
    const strElemType *search_start;
    const strElemType *segment_start;
    const strElemType *search_end;
    const strElemType *found_pos;
    memSizeType pos;
    rtlArrayType resized_result_array;
    rtlArrayType result_array;

  /* strSplit */
    if (likely(ALLOC_RTL_ARRAY(result_array, 256))) {
      result_array->min_position = 1;
      result_array->max_position = 256;
      used_max_position = 0;
      delimiter_size = delimiter->size;
      delimiter_mem = delimiter->mem;
      search_start = mainStri->mem;
      segment_start = search_start;
      if (delimiter_size != 0 && mainStri->size >= delimiter_size) {
        ch_1 = delimiter_mem[0];
        search_end = &mainStri->mem[mainStri->size - delimiter_size + 1];
        while ((found_pos = search_strelem(search_start, ch_1, search_end)) != NULL &&
            result_array != NULL) {
          if (memcmp(found_pos, delimiter_mem,
              delimiter_size * sizeof(strElemType)) == 0) {
            result_array = add_stri_to_array(segment_start,
                (memSizeType) (found_pos - segment_start), result_array,
                &used_max_position);
            search_start = found_pos + delimiter_size;
            segment_start = search_start;
          } else {
            search_start = found_pos + 1;
          } /* if */
        } /* while */
      } /* if */
      if (result_array != NULL) {
        result_array = add_stri_to_array(segment_start,
            (memSizeType) (&mainStri->mem[mainStri->size] - segment_start), result_array,
            &used_max_position);
        if (result_array != NULL) {
          resized_result_array = REALLOC_RTL_ARRAY(result_array,
              (uintType) result_array->max_position, (uintType) used_max_position);
          if (resized_result_array == NULL) {
            for (pos = 0; pos < (uintType) used_max_position; pos++) {
              FREE_STRI(result_array->arr[pos].value.striValue,
                  result_array->arr[pos].value.striValue->size);
            } /* for */
            FREE_RTL_ARRAY(result_array, (uintType) result_array->max_position);
            result_array = NULL;
          } else {
            result_array = resized_result_array;
            COUNT3_RTL_ARRAY((uintType) result_array->max_position, (uintType) used_max_position);
            result_array->max_position = used_max_position;
          } /* if */
        } /* if */
      } /* if */
    } /* if */
    if (unlikely(result_array == NULL)) {
      raise_error(MEMORY_ERROR);
    } /* if */
    return result_array;
  } /* strSplit */



#ifdef ALLOW_STRITYPE_SLICES
/**
 *  Get a substring from a start position with a given length.
 *  The first character in a string has the position 1.
 *  This function is used by the compiler to avoid copiing string data.
 *  The 'slice' is initialized to refer to the substring of 'stri'
 */
void strSubstrSlice (const const_striType stri, intType start, intType length, striType slice)

  {
    memSizeType striSize;

  /* strSubstrSlice */
#ifdef TRACE_STR_RTL
    printf("strSubstrSlice(");
    prot_stri(stri);
    printf(", " FMT_D ", " FMT_D ")", start, length);
    prot_flush();
#endif
    striSize = stri->size;
    if (unlikely(start < 1)) {
      if (length >= 1 && start > 1 - length) {
        length += start - 1;
        start = 1;
      } else {
        length = 0;
      } /* if */
    } /* if */
    SET_SLICE_CAPACITY(slice, 0);
    if (length >= 1 && (uintType) start <= striSize) {
      slice->mem = &stri->mem[start - 1];
      if ((uintType) length > striSize - (memSizeType) start + 1) {
        slice->size = striSize - (memSizeType) start + 1;
      } else {
        slice->size = (memSizeType) length;
      } /* if */
    } else {
      slice->mem = NULL;
      slice->size = 0;
    } /* if */
#ifdef TRACE_STR_RTL
    printf(" --> ");
    prot_stri(slice);
    printf("\n");
#endif
  } /* strSubstrSlice */

#endif



/**
 *  Get a substring from a start position with a given length.
 *  The first character in a string has the position 1.
 *  @return the substring from the start position with a given length.
 *  @exception MEMORY_ERROR Not enough memory to represent the result.
 */
striType strSubstr (const const_striType stri, intType start, intType length)

  {
    memSizeType striSize;
    memSizeType result_size;
    striType result;

  /* strSubstr */
    striSize = stri->size;
    if (unlikely(start < 1)) {
      if (length >= 1 && start > 1 - length) {
        length += start - 1;
        start = 1;
      } else {
        length = 0;
      } /* if */
    } /* if */
    if (length >= 1 && (uintType) start <= striSize) {
      if ((uintType) length > striSize - (memSizeType) start + 1) {
        result_size = striSize - (memSizeType) start + 1;
      } else {
        result_size = (memSizeType) length;
      } /* if */
      if (unlikely(!ALLOC_STRI_SIZE_OK(result, result_size))) {
        raise_error(MEMORY_ERROR);
        return NULL;
      } /* if */
      memcpy(result->mem, &stri->mem[start - 1],
          result_size * sizeof(strElemType));
      result->size = result_size;
    } else {
      if (unlikely(!ALLOC_STRI_SIZE_OK(result, (memSizeType) 0))) {
        raise_error(MEMORY_ERROR);
        return NULL;
      } /* if */
      result->size = 0;
    } /* if */
    return result;
  } /* strSubstr */



#ifdef ALLOW_STRITYPE_SLICES
/**
 *  Get a substring beginning at a start position.
 *  The first character in a 'string' has the position 1.
 *  This function is used by the compiler to avoid copiing string data.
 *  The 'slice' is initialized to refer to the tail of 'stri'
 */
void strTailSlice (const const_striType stri, intType start, striType slice)

  {
    memSizeType striSize;

  /* strTailSlice */
#ifdef TRACE_STR_RTL
    printf("strTailSlice(");
    prot_stri(stri);
    printf(", " FMT_D ")", start);
    prot_flush();
#endif
    striSize = stri->size;
    if (unlikely(start < 1)) {
      start = 1;
    } /* if */
    if ((uintType) start <= striSize && striSize >= 1) {
      SET_SLICE_CAPACITY(slice, 0);
      slice->mem = &stri->mem[start - 1];
      slice->size = striSize - (memSizeType) start + 1;
    } else {
      SET_SLICE_CAPACITY(slice, 0);
      slice->mem = NULL;
      slice->size = 0;
    } /* if */
#ifdef TRACE_STR_RTL
    printf(" --> ");
    prot_stri(slice);
    printf("\n");
#endif
  } /* strTailSlice */

#endif



/**
 *  Get a substring beginning at a start position.
 *  The first character in a 'string' has the position 1.
 *  @return the substring beginning at the start position.
 *  @exception MEMORY_ERROR Not enough memory to represent the result.
 */
striType strTail (const const_striType stri, intType start)

  {
    memSizeType striSize;
    memSizeType result_size;
    striType result;

  /* strTail */
    striSize = stri->size;
    if (unlikely(start < 1)) {
      start = 1;
    } /* if */
    if ((uintType) start <= striSize && striSize >= 1) {
      result_size = striSize - (memSizeType) start + 1;
      if (unlikely(!ALLOC_STRI_SIZE_OK(result, result_size))) {
        raise_error(MEMORY_ERROR);
        return NULL;
      } /* if */
      /* Reversing the order of the following two statements    */
      /* causes an "Internal Compiler Error" with MSC 6.0       */
      /* when using the -Ozacegilt optimisation option in the   */
      /* large memory model (-AL). Note that the order of the   */
      /* two statements make no difference to the logic of the  */
      /* program.                                               */
      memcpy(result->mem, &stri->mem[start - 1],
          result_size * sizeof(strElemType));
      result->size = result_size;
    } else {
      if (unlikely(!ALLOC_STRI_SIZE_OK(result, (memSizeType) 0))) {
        raise_error(MEMORY_ERROR);
        return NULL;
      } /* if */
      result->size = 0;
    } /* if */
    return result;
  } /* strTail */



/**
 *  Convert a string to an UTF-8 encoded string of bytes.
 *  @param stri Normal (UTF-32) string to be converted to UTF-8.
 *  @return 'stri' converted to a string of bytes with UTF-8 encoding.
 */
striType strToUtf8 (const const_striType stri)

  {
    register strElemType *dest;
    register strElemType ch;
    register memSizeType pos;
    memSizeType result_size;
    striType resized_result;
    striType result;

  /* strToUtf8 */
    if (unlikely(stri->size > MAX_STRI_LEN / MAX_UTF8_EXPANSION_FACTOR ||
                 !ALLOC_STRI_SIZE_OK(result, max_utf8_size(stri->size)))) {
      raise_error(MEMORY_ERROR);
      result = NULL;
    } else {
      dest = result->mem;
      for (pos = 0; pos < stri->size; pos++) {
        ch = stri->mem[pos];
        if (ch <= 0x7F) {
          *dest++ = ch;
        } else if (ch <= 0x7FF) {
          dest[0] = 0xC0 | ( ch >>  6);
          dest[1] = 0x80 | ( ch        & 0x3F);
          dest += 2;
        } else if (ch <= 0xFFFF) {
          dest[0] = 0xE0 | ( ch >> 12);
          dest[1] = 0x80 | ((ch >>  6) & 0x3F);
          dest[2] = 0x80 | ( ch        & 0x3F);
          dest += 3;
        } else if (ch <= 0x1FFFFF) {
          dest[0] = 0xF0 | ( ch >> 18);
          dest[1] = 0x80 | ((ch >> 12) & 0x3F);
          dest[2] = 0x80 | ((ch >>  6) & 0x3F);
          dest[3] = 0x80 | ( ch        & 0x3F);
          dest += 4;
        } else if (ch <= 0x3FFFFFF) {
          dest[0] = 0xF8 | ( ch >> 24);
          dest[1] = 0x80 | ((ch >> 18) & 0x3F);
          dest[2] = 0x80 | ((ch >> 12) & 0x3F);
          dest[3] = 0x80 | ((ch >>  6) & 0x3F);
          dest[4] = 0x80 | ( ch        & 0x3F);
          dest += 5;
        } else {
          dest[0] = 0xFC | ( ch >> 30);
          dest[1] = 0x80 | ((ch >> 24) & 0x3F);
          dest[2] = 0x80 | ((ch >> 18) & 0x3F);
          dest[3] = 0x80 | ((ch >> 12) & 0x3F);
          dest[4] = 0x80 | ((ch >>  6) & 0x3F);
          dest[5] = 0x80 | ( ch        & 0x3F);
          dest += 6;
        } /* if */
      } /* for */
      result_size = (memSizeType) (dest - result->mem);
      REALLOC_STRI_SIZE_SMALLER(resized_result, result, max_utf8_size(stri->size), result_size);
      if (unlikely(resized_result == NULL)) {
        FREE_STRI(result, max_utf8_size(stri->size));
        raise_error(MEMORY_ERROR);
        result = NULL;
      } else {
        result = resized_result;
        COUNT3_STRI(max_utf8_size(stri->size), result_size);
        result->size = result_size;
      } /* if */
    } /* if */
    return result;
  } /* strToUtf8 */



/**
 *  Return string with leading and trailing whitespace omitted.
 *  All characters less than or equal to ' ' (space) count as whitespace.
 *  @return string with leading and trailing whitespace omitted.
 */
striType strTrim (const const_striType stri)

  {
    memSizeType start;
    memSizeType striSize;
    striType result;

  /* strTrim */
    start = 0;
    striSize = stri->size;
    if (striSize >= 1) {
      while (start < striSize && stri->mem[start] <= ' ') {
        start++;
      } /* while */
      while (striSize > start && stri->mem[striSize - 1] <= ' ') {
        striSize--;
      } /* while */
      striSize -= start;
    } /* if */
    if (unlikely(!ALLOC_STRI_SIZE_OK(result, striSize))) {
      raise_error(MEMORY_ERROR);
      return NULL;
    } else {
      result->size = striSize;
      memcpy(result->mem, &stri->mem[start], striSize * sizeof(strElemType));
      return result;
    } /* if */
  } /* strTrim */



/**
 *  Convert a string to upper case.
 *  The conversion uses the default Unicode case mapping,
 *  where each character is considered in isolation.
 *  Characters without case mapping are left unchanged.
 *  The mapping is independend from the locale. Individual
 *  character case mappings cannot be reversed, because some
 *  characters have multiple characters that map to them.
 *  @return the string converted to upper case.
 */
striType strUp (const const_striType stri)

  {
    memSizeType striSize;
    striType result;

  /* strUp */
    striSize = stri->size;
    if (unlikely(!ALLOC_STRI_SIZE_OK(result, striSize))) {
      raise_error(MEMORY_ERROR);
      return NULL;
    } else {
      result->size = striSize;
      toUpper(stri->mem, stri->size, result->mem);
      return result;
    } /* if */
  } /* strUp */



/**
 *  Convert a string to upper case.
 *  StrUpTemp is used by the compiler when 'stri' is temporary
 *  value that can be reused.
 *  @return the string converted to lower case.
 */
striType strUpTemp (const striType stri)

  { /* strUpTemp */
    toUpper(stri->mem, stri->size, stri->mem);
    return stri;
  } /* strUpTemp */



/**
 *  Convert a string with bytes in UTF-8 encoding to UTF-32.
 *  @param utf8 String of bytes encoded with UTF-8.
 *  @return 'utf8' converted to a normal (UTF-32) string.
 *  @exception RANGE_ERROR When characters beyond '\255;' are present or
 *                         when 'utf8' is not encoded with UTF-8.
 */
striType strUtf8ToStri (const_striType utf8)

  {
    memSizeType striSize;
    memSizeType pos;
    const strElemType *utf8ptr;
    boolType okay = TRUE;
    striType resized_result;
    striType result;

  /* strUtf8ToStri */
    striSize = utf8->size;
    if (unlikely(!ALLOC_STRI_SIZE_OK(result, striSize))) {
      raise_error(MEMORY_ERROR);
    } else {
      utf8ptr = &utf8->mem[0];
      pos = 0;
      for (; striSize > 0; pos++, striSize--) {
        if (*utf8ptr <= 0x7F) {
          result->mem[pos] = *utf8ptr++;
        } else if (utf8ptr[0] >= 0xC0 && utf8ptr[0] <= 0xDF && striSize >= 2 &&
                   utf8ptr[1] >= 0x80 && utf8ptr[1] <= 0xBF) {
          /* utf8ptr[0]   range 192 to 223 (leading bits 110.....) */
          /* utf8ptr[1]   range 128 to 191 (leading bits 10......) */
          result->mem[pos] = (utf8ptr[0] & 0x1F) << 6 |
                             (utf8ptr[1] & 0x3F);
          utf8ptr += 2;
          striSize--;
        } else if (utf8ptr[0] >= 0xE0 && utf8ptr[0] <= 0xEF && striSize >= 3 &&
                   utf8ptr[1] >= 0x80 && utf8ptr[1] <= 0xBF &&
                   utf8ptr[2] >= 0x80 && utf8ptr[2] <= 0xBF) {
          /* utf8ptr[0]   range 224 to 239 (leading bits 1110....) */
          /* utf8ptr[1..] range 128 to 191 (leading bits 10......) */
          result->mem[pos] = (utf8ptr[0] & 0x0F) << 12 |
                             (utf8ptr[1] & 0x3F) <<  6 |
                             (utf8ptr[2] & 0x3F);
          utf8ptr += 3;
          striSize -= 2;
        } else if (utf8ptr[0] >= 0xF0 && utf8ptr[0] <= 0xF7 && striSize >= 4 &&
                   utf8ptr[1] >= 0x80 && utf8ptr[1] <= 0xBF &&
                   utf8ptr[2] >= 0x80 && utf8ptr[2] <= 0xBF &&
                   utf8ptr[3] >= 0x80 && utf8ptr[3] <= 0xBF) {
          /* utf8ptr[0]   range 240 to 247 (leading bits 11110...) */
          /* utf8ptr[1..] range 128 to 191 (leading bits 10......) */
          result->mem[pos] = (utf8ptr[0] & 0x07) << 18 |
                             (utf8ptr[1] & 0x3F) << 12 |
                             (utf8ptr[2] & 0x3F) <<  6 |
                             (utf8ptr[3] & 0x3F);
          utf8ptr += 4;
          striSize -= 3;
        } else if (utf8ptr[0] >= 0xF8 && utf8ptr[0] <= 0xFB && striSize >= 5 &&
                   utf8ptr[1] >= 0x80 && utf8ptr[1] <= 0xBF &&
                   utf8ptr[2] >= 0x80 && utf8ptr[2] <= 0xBF &&
                   utf8ptr[3] >= 0x80 && utf8ptr[3] <= 0xBF &&
                   utf8ptr[4] >= 0x80 && utf8ptr[4] <= 0xBF) {
          /* utf8ptr[0]   range 248 to 251 (leading bits 111110..) */
          /* utf8ptr[1..] range 128 to 191 (leading bits 10......) */
          result->mem[pos] = (utf8ptr[0] & 0x03) << 24 |
                             (utf8ptr[1] & 0x3F) << 18 |
                             (utf8ptr[2] & 0x3F) << 12 |
                             (utf8ptr[3] & 0x3F) <<  6 |
                             (utf8ptr[4] & 0x3F);
          utf8ptr += 5;
          striSize -= 4;
        } else if (utf8ptr[0] >= 0xFC && utf8ptr[0] <= 0xFF && striSize >= 6 &&
                   utf8ptr[1] >= 0x80 && utf8ptr[1] <= 0xBF &&
                   utf8ptr[2] >= 0x80 && utf8ptr[2] <= 0xBF &&
                   utf8ptr[3] >= 0x80 && utf8ptr[3] <= 0xBF &&
                   utf8ptr[4] >= 0x80 && utf8ptr[4] <= 0xBF &&
                   utf8ptr[5] >= 0x80 && utf8ptr[5] <= 0xBF) {
          /* utf8ptr[0]   range 252 to 255 (leading bits 111111..) */
          /* utf8ptr[1..] range 128 to 191 (leading bits 10......) */
          result->mem[pos] = (utf8ptr[0] & 0x03) << 30 |
                             (utf8ptr[1] & 0x3F) << 24 |
                             (utf8ptr[2] & 0x3F) << 18 |
                             (utf8ptr[3] & 0x3F) << 12 |
                             (utf8ptr[4] & 0x3F) <<  6 |
                             (utf8ptr[5] & 0x3F);
          utf8ptr += 6;
          striSize -= 5;
        } else {
          /* utf8ptr[0] not in range 0xC0 to 0xFF (192 to 255) */
          /* or not enough continuation bytes found.           */
          okay = FALSE;
          striSize = 1;
        } /* if */
      } /* for */
      if (likely(okay)) {
        REALLOC_STRI_SIZE_SMALLER(resized_result, result, utf8->size, pos);
        if (unlikely(resized_result == NULL)) {
          FREE_STRI(result, utf8->size);
          raise_error(MEMORY_ERROR);
          result = NULL;
        } else {
          result = resized_result;
          COUNT3_STRI(utf8->size, pos);
          result->size = pos;
        } /* if */
      } else {
        FREE_STRI(result, utf8->size);
        raise_error(RANGE_ERROR);
        result = NULL;
      } /* if */
    } /* if */
    return result;
  } /* strUtf8ToStri */



striType strZero (const intType factor)

  {
    striType result;

  /* strZero */
#ifdef TRACE_STR_RTL
    printf("strZero(" FMT_D ")\n", factor);
#endif
    if (unlikely(factor < 0)) {
      raise_error(RANGE_ERROR);
      result = NULL;
    } else {
      if (unlikely((uintType) factor > MAX_STRI_LEN ||
                   !ALLOC_STRI_SIZE_OK(result, (memSizeType) factor))) {
        raise_error(MEMORY_ERROR);
        result = NULL;
      } else {
        result->size = (memSizeType) factor;
        memset(result->mem, 0, (memSizeType) factor * sizeof(strElemType));
      } /* if */
    } /* if */
    return result;
  } /* strZero */
