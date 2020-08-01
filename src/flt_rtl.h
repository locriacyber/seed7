/********************************************************************/
/*                                                                  */
/*  flt_rtl.h     Primitive actions for the float type.             */
/*  Copyright (C) 1989 - 2010  Thomas Mertes                        */
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
/*  File: seed7/src/flt_rtl.h                                       */
/*  Changes: 1993, 1994, 2005, 2010  Thomas Mertes                  */
/*  Content: Primitive actions for the float type.                  */
/*                                                                  */
/********************************************************************/

#ifdef ISNAN_WITH_UNDERLINE
#define isnan _isnan
#endif


#ifdef ANSI_C

void setupFloat (void);
inttype fltCmp (floattype number1, floattype number2);
void fltCpy (floattype *dest, floattype source);
stritype fltDgts (floattype number, inttype digits_precision);
#ifdef NAN_COMPARISON_WRONG
booltype fltEq (floattype number1, floattype number2);
booltype fltGe (floattype number1, floattype number2);
booltype fltGt (floattype number1, floattype number2);
#endif
floattype fltIPow (floattype base, inttype exponent);
booltype fltIsNegativeZero (floattype number);
#ifdef NAN_COMPARISON_WRONG
booltype fltLe (floattype number1, floattype number2);
booltype fltLt (floattype number1, floattype number2);
#endif
floattype fltParse (const const_stritype stri);
#ifdef POWER_OF_ZERO_WRONG
floattype fltPow (floattype base, floattype exponent);
#endif
floattype fltRand (floattype lower_limit, floattype upper_limit);
stritype fltStr (floattype number);

#else

void setupFloat ();
inttype fltCmp ();
void fltCpy ();
stritype fltDgts ();
#ifdef NAN_COMPARISON_WRONG
booltype fltEq ();
booltype fltGe ();
booltype fltGt ();
#endif
floattype fltIPow ();
booltype fltIsNegativeZero ();
#ifdef NAN_COMPARISON_WRONG
booltype fltLe ();
booltype fltLt ();
#endif
floattype fltParse ();
#ifdef POWER_OF_ZERO_WRONG
floattype fltPow ();
#endif
floattype fltRand ();
stritype fltStr ();

#endif
