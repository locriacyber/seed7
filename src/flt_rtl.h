/********************************************************************/
/*                                                                  */
/*  flt_rtl.h     Primitive actions for the float type.             */
/*  Copyright (C) 1989 - 2005  Thomas Mertes                        */
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
/*  File: seed7/src/flt_rtl.h                                       */
/*  Changes: 1993, 1994, 2005  Thomas Mertes                        */
/*  Content: Primitive actions for the float type.                  */
/*                                                                  */
/********************************************************************/

/* The function fltCreate is used from hashtables where the keys    */
/* and the data is stored in integer data elements. On some         */
/* architectures (linux with gcc) functions with float results seem */
/* to be returned in a different way (may be another register).     */
/* Therefore fltCreate uses generictype instead of floattype.       */
/* Here is the place to decide if fltCreate should use floattype    */
/* or inttype.                                                      */
typedef inttype generictype;

#ifdef DEFINE_IEEE_754_STUFF
int _isnan( double x );
#define isnan _isnan
#endif


#ifdef ANSI_C

inttype fltCmp (floattype, floattype);
void fltCpy (floattype *, floattype);
generictype fltCreate (generictype);
stritype fltDgts (floattype, inttype);
floattype fltIPow (floattype, inttype);
floattype fltParse (stritype);
floattype fltRand (floattype, floattype);
stritype fltStr (floattype);

#else

inttype fltCmp ();
void fltCpy ();
floattype fltCreate ();
stritype fltDgts ();
floattype fltIPow ();
floattype fltParse ();
floattype fltRand ();
stritype fltStr ();

#endif
