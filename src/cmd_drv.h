/********************************************************************/
/*                                                                  */
/*  cmd_drv.h     Prototypes of OS specific command functions.      */
/*  Copyright (C) 1989 - 2011  Thomas Mertes                        */
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
/*  File: seed7/src/cmd_drv.h                                       */
/*  Changes: 2010  Thomas Mertes                                    */
/*  Content: Prototypes of OS specific command functions.           */
/*                                                                  */
/********************************************************************/

#ifdef MAP_ABSOLUTE_PATH_TO_DRIVE_LETTERS
typedef struct {
    uint32Type magicValue;
    uint32Type driveBitmask;
    int currentDrive;
  } volumeListType;

#define IS_VOLUME_LIST(ptr) (ptr != NULL && ((volumeListType *) (ptr))->magicValue == UINT32TYPE_MAX)
#endif


#ifdef C_PLUS_PLUS
#define C "C"
#else
#define C
#endif

#ifdef DEFINE_OS_STAT_PROTOTYPE
extern C int __cdecl os_stat (const_os_striType path, os_stat_struct *buffer);
#endif


os_striType *getUtf16Argv (int *w_argc);
void freeUtf16Argv (os_striType *w_argv);
striType getExecutablePath (const const_striType arg_0);
#ifdef DEFINE_WGETENV
os_striType wgetenv (const const_os_striType name);
#endif
#ifdef DEFINE_WSETENV
int wsetenv (const const_os_striType name, const const_os_striType value,
    int overwrite);
#endif
#ifdef MAP_ABSOLUTE_PATH_TO_DRIVE_LETTERS
volumeListType *openVolumeList (void);
#endif
