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
/*  File: seed7/src/match.h                                         */
/*  Changes: 1995, 1999, 2000  Thomas Mertes                        */
/*  Content: Procedures to do static match on expressions.          */
/*                                                                  */
/********************************************************************/

#ifdef ANSI_C

void update_owner (const_objecttype);
objecttype match_object (objecttype);
objecttype match_expression (objecttype);
objecttype match_prog_expression (const_nodetype, objecttype);

#else

void update_owner ();
objecttype match_object ();
objecttype match_expression ();
objecttype match_prog_expression ();

#endif
