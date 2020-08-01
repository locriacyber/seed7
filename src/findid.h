/********************************************************************/
/*                                                                  */
/*  hi   Interpreter for Seed7 programs.                            */
/*  Copyright (C) 1990 - 2000  Thomas Mertes                        */
/*                                                                  */
/*  This program is free software; you can redistribute it and/or   */
/*  modify it under the terms of the GNU General Public License as  */
/*  published by the Free Software Foundation; either version 2 of  */
/*  the License, or (at your option) any later version.             */
/*                                                                  */
/*  This program is distributed in the hope that it will be useful, */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of  */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the   */
/*  GNU General Public License for more details.                    */
/*                                                                  */
/*  You should have received a copy of the GNU General Public       */
/*  License along with this program; if not, write to the Free      */
/*  Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,  */
/*  MA 02111-1307 USA                                               */
/*                                                                  */
/*  Module: Analyzer                                                */
/*  File: seed7/src/findid.h                                        */
/*  Changes: 1991, 1992, 1993, 1994  Thomas Mertes                  */
/*  Content: Procedures to maintain the identifier table.           */
/*                                                                  */
/********************************************************************/

#define STRONGEST_PRIORITY ((prior_type)   0)
#define SCOL_PRIORITY      ((prior_type)  49)
#define COM_PRIORITY       ((prior_type)  99)
#define WEAKEST_PRIORITY   ((prior_type) 127)

EXTERN identtype current_ident;


#define find_1_ch_ident(CH)  current_ident = prog.ident.table1[(CH)]
#define find_literal_ident() current_ident = prog.ident.literal
#define find_eof_ident()     current_ident = prog.ident.end_of_file


#ifdef ANSI_C

void find_normal_ident (unsigned int);
void check_list_of_syntax_elements (const_listtype);
void clean_idents (void);
void write_idents (void);
void init_findid (errinfotype *);

#else

void find_normal_ident ();
void check_list_of_syntax_elements ();
void clean_idents ();
void write_idents ();
void init_findid ();

#endif
