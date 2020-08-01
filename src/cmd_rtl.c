/********************************************************************/
/*                                                                  */
/*  cmd_rtl.c     Primitive actions for various commands.           */
/*  Copyright (C) 1989 - 2009  Thomas Mertes                        */
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
/*  File: seed7/src/cmd_rtl.c                                       */
/*  Changes: 1994, 2006, 2009  Thomas Mertes                        */
/*  Content: Primitive actions for various commands.                */
/*                                                                  */
/********************************************************************/

#include "version.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "limits.h"
#include "time.h"
#include "sys/types.h"
#include "sys/stat.h"
#ifdef INCLUDE_SYS_UTIME
#include "sys/utime.h"
#else
#include "utime.h"
#endif
#ifdef OS_PATH_WCHAR
#include "wchar.h"
#endif
#include "errno.h"

#ifdef USE_MYUNISTD_H
#include "myunistd.h"
#else
#include "unistd.h"
#endif

#include "dir_drv.h"

#include "common.h"
#include "data_rtl.h"
#include "heaputl.h"
#include "striutl.h"
#include "str_rtl.h"
#include "fil_rtl.h"
#include "dir_rtl.h"
#include "tim_rtl.h"
#include "big_drv.h"
#include "rtl_err.h"

#ifdef USE_MMAP
#include "sys/mman.h"
#endif

#undef EXTERN
#define EXTERN
#include "cmd_rtl.h"

#undef TRACE_CMD_RTL


#ifndef PATH_MAX
#define PATH_MAX 2048
#endif

#define SIZE_NORMAL_BUFFER   32768
#define SIZE_RESERVE_BUFFER   2048

#define PRESERVE_NOTHING    0x00
#define PRESERVE_MODE       0x01
#define PRESERVE_OWNERSHIP  0x02
#define PRESERVE_TIMESTAMPS 0x04
#define PRESERVE_SYMLINKS   0x08
#define PRESERVE_ALL        0xFF

#define MAX_SYMLINK_PATH_LENGTH 0x1FFFFFFF



#ifdef ANSI_C

#ifdef USE_CDECL
static int _cdecl cmp_mem (char *strg1, char *strg2)
#else
static int cmp_mem (void const *strg1, void const *strg2)
#endif
#else

static int cmp_mem (strg1, strg2)
char *strg1;
char *strg2;
#endif

  { /* cmp_mem */
    return(strCompare(
        ((rtlObjecttype *) strg1)->value.strivalue,
        ((rtlObjecttype *) strg2)->value.strivalue));
  } /* cmp_mem */



#ifdef ANSI_C
static void remove_any_file (os_path_stri, errinfotype *);
static void copy_any_file (os_path_stri, os_path_stri, int, errinfotype *);
#else
static void remove_any_file ();
static void copy_any_file ();
#endif



#ifdef ANSI_C

static void remove_dir (os_path_stri dir_name, errinfotype *err_info)
#else

static void remove_dir (dir_name, err_info)
os_path_stri dir_name;
errinfotype *err_info;
#endif

  {
    os_DIR *directory;
    os_dirent_struct *current_entry;
    size_t dir_name_size;
    size_t dir_path_capacity = 0;
    os_path_stri dir_path = NULL;
    size_t new_size;
    os_path_stri resized_path;
    booltype init_path = TRUE;

  /* remove_dir */
#ifdef TRACE_CMD_RTL
    printf("BEGIN remove_dir(\"%s\")\n", dir_name);
#endif
/*  printf("opendir(%s);\n", dir_name);
    fflush(stdout); */
    if ((directory = os_opendir(dir_name)) == NULL) {
      *err_info = FILE_ERROR;
    } else {
      do {
        current_entry = os_readdir(directory);
/*      printf("$%ld$\n", (long) current_entry);
        fflush(stdout); */
      } while (current_entry != NULL &&
          (memcmp(current_entry->d_name, dot,    sizeof(os_path_char) * 2) == 0 ||
           memcmp(current_entry->d_name, dotdot, sizeof(os_path_char) * 3) == 0));
      dir_name_size = os_path_strlen(dir_name);
      while (*err_info == OKAY_NO_ERROR && current_entry != NULL) {
/*      printf("!%s!\n", current_entry->d_name);
        fflush(stdout); */
        new_size = dir_name_size + 1 + os_path_strlen(current_entry->d_name);
        if (new_size > dir_path_capacity) {
          resized_path = os_path_realloc(dir_path, new_size);
          if (resized_path != NULL) {
            dir_path = resized_path;
            dir_path_capacity = new_size;
          } else if (dir_path != NULL) {
            os_path_free(dir_path);
            dir_path = NULL;
          } /* if */
        } /* if */
        if (dir_path != NULL) {
          if (init_path) {
            os_path_strcpy(dir_path, dir_name);
            os_path_strcpy(&dir_path[dir_name_size], slash);
            init_path = FALSE;
          } /* if */
          os_path_strcpy(&dir_path[dir_name_size + 1], current_entry->d_name);
          remove_any_file(dir_path, err_info);
        } else {
          *err_info = MEMORY_ERROR;
        } /* if */
        do {
          current_entry = os_readdir(directory);
/*        printf("$%ld$\n", (long) current_entry);
          fflush(stdout); */
        } while (current_entry != NULL &&
            (memcmp(current_entry->d_name, dot,    sizeof(os_path_char) * 2) == 0 ||
             memcmp(current_entry->d_name, dotdot, sizeof(os_path_char) * 3) == 0));
      } /* while */
      if (dir_path != NULL) {
        os_path_free(dir_path);
      } /* if */
      os_closedir(directory);
      if (*err_info == OKAY_NO_ERROR) {
        /* printf("before remove directory <%s>\n", dir_name); */
        if (os_rmdir(dir_name) != 0) {
          *err_info = FILE_ERROR;
        } /* if */
/*      okay = errno;
        printf("errno=%d\n", okay);
        printf("EACCES=%d  EBUSY=%d  EEXIST=%d  ENOTEMPTY=%d  ENOENT=%d  ENOTDIR=%d  EROFS=%d\n",
            EACCES, EBUSY, EEXIST, ENOTEMPTY, ENOENT, ENOTDIR, EROFS); */
        /* printf("remove ==> %d\n", remove(dir_name)); */
      } /* if */
    } /* if */
#ifdef TRACE_CMD_RTL
    printf("END remove_dir(\"%s\", %d)\n", dir_name, *err_info);
#endif
  } /* remove_dir */



#ifdef ANSI_C

static void remove_any_file (os_path_stri file_name, errinfotype *err_info)
#else

static void remove_any_file (file_name, err_info)
os_path_stri file_name;
errinfotype *err_info;
#endif

  {
    os_stat_struct file_stat;

  /* remove_any_file */
#ifdef TRACE_CMD_RTL
    printf("BEGIN remove_any_file(\"%s\")\n", file_name);
#endif
    if (os_lstat(file_name, &file_stat) != 0) {
      *err_info = FILE_ERROR;
    } else {
      if (S_ISLNK(file_stat.st_mode)) {
        if (os_remove(file_name) != 0) {
          *err_info = FILE_ERROR;
        } /* if */
      } else if (S_ISREG(file_stat.st_mode)) {
        if (os_remove(file_name) != 0) {
          *err_info = FILE_ERROR;
        } /* if */
      } else if (S_ISDIR(file_stat.st_mode)) {
        remove_dir(file_name, err_info);
      } /* if */
    } /* if */
#ifdef TRACE_CMD_RTL
    printf("END remove_any_file(\"%s\", %d)\n", file_name, *err_info);
#endif
  } /* remove_any_file */



#ifdef ANSI_C

static void copy_file (os_path_stri from_name, os_path_stri to_name, errinfotype *err_info)
#else

static void copy_file (from_name, to_name, err_info)
os_path_stri from_name;
os_path_stri to_name;
errinfotype *err_info;
#endif

  {
    FILE *from_file;
    FILE *to_file;
#ifdef USE_MMAP
    os_stat_struct file_stat;
    memsizetype file_length;
    ustritype file_content;
#else
    char *buffer;
    size_t buffer_size;
    char reserve_buffer[SIZE_RESERVE_BUFFER];
    size_t bytes_read;
#endif

  /* copy_file */
#ifdef TRACE_CMD_RTL
    printf("BEGIN copy_file(\"%s\", \"%s\")\n", from_name, to_name);
#endif
#ifdef OS_PATH_WCHAR
    if ((from_file = wide_fopen(from_name, L"rb")) != NULL) {
      if ((to_file = wide_fopen(to_name, L"wb")) != NULL) {
#else
    if ((from_file = fopen(from_name, "rb")) != NULL) {
      if ((to_file = fopen(to_name, "wb")) != NULL) {
#endif
#ifdef USE_MMAP
        if (os_fstat(fileno(from_file), &file_stat) == 0) {
          file_length = file_stat.st_size;
          if ((file_content = (ustritype) mmap(NULL, file_length,
              PROT_READ, MAP_PRIVATE, fileno(from_file),
              0)) != (ustritype) -1) {
            if (fwrite(file_content, 1, file_length, to_file) != file_length) {
              *err_info = FILE_ERROR;
            } /* if */
          } /* if */
        } /* if */
#else
        if (ALLOC_BYTES(buffer, SIZE_NORMAL_BUFFER)) {
          buffer_size = SIZE_NORMAL_BUFFER;
        } else {
          buffer = reserve_buffer;
          buffer_size = SIZE_RESERVE_BUFFER;
        } /* if */
        while (*err_info == OKAY_NO_ERROR && (bytes_read =
            fread(buffer, 1, buffer_size, from_file)) != 0) {
          if (fwrite(buffer, 1, bytes_read, to_file) != bytes_read) {
            *err_info = FILE_ERROR;
          } /* if */
        } /* while */
        if (buffer != reserve_buffer) {
          FREE_BYTES(buffer, SIZE_NORMAL_BUFFER);
        } /* if */
#endif
        if (fclose(from_file) != 0) {
          *err_info = FILE_ERROR;
        } /* if */
        if (fclose(to_file) != 0) {
          *err_info = FILE_ERROR;
        } /* if */
        if (*err_info != OKAY_NO_ERROR) {
          os_remove(to_name);
        } /* if */
      } else {
        fclose(from_file);
        *err_info = FILE_ERROR;
      } /* if */
    } else {
      *err_info = FILE_ERROR;
    } /* if */
#ifdef TRACE_CMD_RTL
    printf("END copy_file(\"%s\", \"%s\", %d)\n",
        from_name, to_name, *err_info);
#endif
  } /* copy_file */



#ifdef ANSI_C

static void copy_dir (os_path_stri from_name, os_path_stri to_name,
    int flags, errinfotype *err_info)
#else

static void copy_dir (from_name, to_name, flags, err_info)
os_path_stri from_name;
os_path_stri to_name;
 int flags;
errinfotype *err_info;
#endif

  {
    os_DIR *directory;
    os_dirent_struct *current_entry;
    size_t from_name_size;
    size_t to_name_size;
    size_t d_name_size;
    size_t from_path_capacity = 0;
    os_path_stri from_path = NULL;
    size_t to_path_capacity = 0;
    os_path_stri to_path = NULL;
    size_t new_size;
    os_path_stri resized_path;
    booltype init_path = TRUE;

  /* copy_dir */
#ifdef TRACE_CMD_RTL
    printf("BEGIN copy_dir(\"%s\", \"%s\")\n", from_name, to_name);
#endif
    /* printf("opendir(%s);\n", from_name);
       fflush(stdout); */
    if ((directory = os_opendir(from_name)) == NULL) {
      *err_info = FILE_ERROR;
    } else {
      if (os_mkdir(to_name, (S_IRWXU | S_IRWXG | S_IRWXO)) != 0) {
        *err_info = FILE_ERROR;
      } else {
        do {
          current_entry = os_readdir(directory);
          /* printf("$%ld$\n", (long) current_entry);
             fflush(stdout); */
        } while (current_entry != NULL &&
            (memcmp(current_entry->d_name, dot,    sizeof(os_path_char) * 2) == 0 ||
             memcmp(current_entry->d_name, dotdot, sizeof(os_path_char) * 3) == 0));
        from_name_size = os_path_strlen(from_name);
        to_name_size = os_path_strlen(to_name);
        while (*err_info == OKAY_NO_ERROR && current_entry != NULL) {
          /* printf("!%s!\n", current_entry->d_name);
             fflush(stdout); */
          d_name_size = os_path_strlen(current_entry->d_name);
          new_size = from_name_size + 1 + d_name_size;
          if (new_size > from_path_capacity) {
            resized_path = os_path_realloc(from_path, new_size);
            if (resized_path != NULL) {
              from_path = resized_path;
              from_path_capacity = new_size;
            } else if (from_path != NULL) {
              os_path_free(from_path);
              from_path = NULL;
            } /* if */
          } /* if */
          new_size = to_name_size + 1 + d_name_size;
          if (new_size > to_path_capacity) {
            resized_path = os_path_realloc(to_path, new_size);
            if (resized_path != NULL) {
              to_path = resized_path;
              to_path_capacity = new_size;
            } else if (to_path != NULL) {
              os_path_free(to_path);
              to_path = NULL;
            } /* if */
          } /* if */
          if (from_path != NULL && to_path != NULL) {
            if (init_path) {
              os_path_strcpy(from_path, from_name);
              os_path_strcpy(&from_path[from_name_size], slash);
              os_path_strcpy(to_path, to_name);
              os_path_strcpy(&to_path[to_name_size], slash);
              init_path = FALSE;
            } /* if */
            os_path_strcpy(&from_path[from_name_size + 1], current_entry->d_name);
            os_path_strcpy(&to_path[to_name_size + 1], current_entry->d_name);
            copy_any_file(from_path, to_path, flags, err_info);
          } else {
            *err_info = MEMORY_ERROR;
          } /* if */
          do {
            current_entry = os_readdir(directory);
            /* printf("$%ld$\n", (long) current_entry);
               fflush(stdout); */
          } while (current_entry != NULL &&
              (memcmp(current_entry->d_name, dot,    sizeof(os_path_char) * 2) == 0 ||
               memcmp(current_entry->d_name, dotdot, sizeof(os_path_char) * 3) == 0));
        } /* while */
        if (*err_info != OKAY_NO_ERROR) {
          remove_dir(to_name, err_info);
        } /* if */
        if (from_path != NULL) {
          os_path_free(from_path);
        } /* if */
        if (to_path != NULL) {
          os_path_free(to_path);
        } /* if */
      } /* if */
      os_closedir(directory);
    } /* if */
#ifdef TRACE_CMD_RTL
    printf("END copy_dir(\"%s\", \"%s\", %d)\n",
        from_name, to_name, *err_info);
#endif
  } /* copy_dir */



#ifdef ANSI_C

static void copy_any_file (os_path_stri from_name, os_path_stri to_name,
    int flags, errinfotype *err_info)
#else

static void copy_any_file (from_name, to_name, flags, err_info)
os_path_stri from_name;
os_path_stri to_name;
int flags;
errinfotype *err_info;
#endif

  {
    os_stat_struct from_stat;
    os_stat_struct to_stat;
    int from_stat_result;
#ifdef HAS_SYMLINKS
    os_path_stri link_destination;
    ssize_t readlink_result;
#endif
    os_utimbuf_struct to_utime;

  /* copy_any_file */
#ifdef TRACE_CMD_RTL
    printf("BEGIN copy_any_file(\"%s\", \"%s\")\n", from_name, to_name);
#endif
    if (flags & PRESERVE_SYMLINKS) {
      from_stat_result = os_lstat(from_name, &from_stat);
    } else {
      from_stat_result = os_stat(from_name, &from_stat);
    } /* if */
    if (from_stat_result != 0) {
      *err_info = FILE_ERROR;
    } else {
      if (os_stat(to_name, &to_stat) == 0) {
        /* File exists already */
        *err_info = FILE_ERROR;
      } else {
        if (S_ISLNK(from_stat.st_mode)) {
#ifdef HAS_SYMLINKS
          /* printf("link size=%lu\n", from_stat.st_size); */
          if (from_stat.st_size > MAX_SYMLINK_PATH_LENGTH || from_stat.st_size < 0) {
            *err_info = RANGE_ERROR;
          } else {
            if (!os_path_alloc(link_destination, from_stat.st_size)) {
              *err_info = MEMORY_ERROR;
            } else {
              readlink_result = readlink(from_name, link_destination,
                                         (size_t) from_stat.st_size);
              if (readlink_result != -1) {
                link_destination[readlink_result] = '\0';
                /* printf("readlink_result=%lu\n", readlink_result);
                   printf("link=%s\n", link_destination); */
                if (symlink(link_destination, to_name) != 0) {
                  *err_info = FILE_ERROR;
                } /* if */
              } else {
                *err_info = FILE_ERROR;
              } /* if */
              os_path_free(link_destination);
            } /* if */
          } /* if */
#else
          *err_info = FILE_ERROR;
#endif
        } else if (S_ISREG(from_stat.st_mode)) {
          copy_file(from_name, to_name, err_info);
        } else if (S_ISDIR(from_stat.st_mode)) {
          copy_dir(from_name, to_name, flags, err_info);
        } /* if */
        if (*err_info == OKAY_NO_ERROR && !S_ISLNK(from_stat.st_mode)) {
          if (flags & PRESERVE_MODE) {
            os_chmod(to_name, from_stat.st_mode);
          } /* if */
          if (flags & PRESERVE_OWNERSHIP) {
            os_chown(to_name, from_stat.st_uid, from_stat.st_gid);
          } /* if */
          if (flags & PRESERVE_TIMESTAMPS) {
            to_utime.actime = from_stat.st_atime;
            to_utime.modtime = from_stat.st_mtime;
            os_utime(to_name, &to_utime);
          } /* if */
        } /* if */
      } /* if */
    } /* if */
#ifdef TRACE_CMD_RTL
    printf("END copy_any_file(\"%s\", \"%s\", %d)\n",
        from_name, to_name, *err_info);
#endif
  } /* copy_any_file */



#ifdef ANSI_C

static void move_any_file (os_path_stri from_name, os_path_stri to_name, errinfotype *err_info)
#else

static void move_any_file (from_name, to_name, err_info)
os_path_stri from_name;
os_path_stri to_name;
errinfotype *err_info;
#endif

  { /* move_any_file */
#ifdef TRACE_CMD_RTL
    printf("BEGIN move_any_file(\"%s\", \"%s\")\n", from_name, to_name);
#endif
    if (os_rename(from_name, to_name) != 0) {
      switch (errno) {
        case EXDEV:
          copy_any_file(from_name, to_name, PRESERVE_ALL, err_info);
          if (*err_info == OKAY_NO_ERROR) {
            remove_any_file(from_name, err_info);
          } else {
            remove_any_file(to_name, err_info);
          } /* if */
          break;
      } /* switch */
    } /* if */
#ifdef TRACE_CMD_RTL
    printf("END move_any_file(\"%s\", \"%s\", %d)\n",
        from_name, to_name, *err_info);
#endif
  } /* move_any_file */



#ifdef ANSI_C

static rtlArraytype read_dir (stritype dir_name, errinfotype *err_info)
#else

static rtlArraytype read_dir (dir_name, err_info)
stritype dir_name;
errinfotype *err_info;
#endif

  {
    rtlArraytype dir_array;
    rtlArraytype resized_dir_array;
    memsizetype max_array_size;
    memsizetype used_array_size;
    memsizetype position;
    dirtype directory;
    stritype stri1;

  /* read_dir */
/*  printf("opendir(");
    prot_stri(dir_name);
    printf(");\n");
    fflush(stdout); */
    if ((directory = dirOpen(dir_name)) != NULL) {
      max_array_size = 256;
      if (ALLOC_RTL_ARRAY(dir_array, max_array_size)) {
        used_array_size = 0;
        stri1 = dirRead(directory);
        while (*err_info == OKAY_NO_ERROR && stri1 != NULL) {
          if (used_array_size >= max_array_size) {
            resized_dir_array = REALLOC_RTL_ARRAY(dir_array,
                max_array_size, max_array_size + 256);
            if (resized_dir_array == NULL) {
              *err_info = MEMORY_ERROR;
            } else {
              dir_array = resized_dir_array;
              COUNT3_RTL_ARRAY(max_array_size, max_array_size + 256);
              max_array_size += 256;
            } /* if */
          } /* if */
          if (*err_info == OKAY_NO_ERROR) {
            dir_array->arr[(int) used_array_size].value.strivalue = stri1;
            used_array_size++;
            stri1 = dirRead(directory);
          } /* if */
        } /* while */
        if (*err_info == OKAY_NO_ERROR) {
          resized_dir_array = REALLOC_RTL_ARRAY(dir_array,
              max_array_size, used_array_size);
          if (resized_dir_array == NULL) {
            *err_info = MEMORY_ERROR;
          } else {
            dir_array = resized_dir_array;
            COUNT3_RTL_ARRAY(max_array_size, used_array_size);
            dir_array->min_position = 1;
            dir_array->max_position = used_array_size;
          } /* if */
        } /* if */
        if (*err_info != OKAY_NO_ERROR) {
          for (position = 0; position < used_array_size; position++) {
            FREE_STRI(dir_array->arr[(int) position].value.strivalue,
                dir_array->arr[(int) position].value.strivalue->size);
          } /* for */
          FREE_RTL_ARRAY(dir_array, max_array_size);
          dir_array = NULL;
        } /* if */
      } else {
        *err_info = MEMORY_ERROR;
      } /* if */
      dirClose(directory);
    } else {
      dir_array = NULL;
      *err_info = FILE_ERROR;
    } /* if */
    return(dir_array);
  } /* read_dir */



#ifdef ANSI_C

biginttype cmdBigFileSize (stritype file_name)
#else

biginttype cmdBigFileSize (file_name)
stritype file_name;
#endif

  {
    os_path_stri os_path;
    os_stat_struct stat_buf;
    int stat_result;
    filetype aFile;
    errinfotype err_info = OKAY_NO_ERROR;
    biginttype result;

  /* cmdBigFileSize */
    os_path = cp_to_os_path(file_name, &err_info);
    if (os_path == NULL) {
      result = NULL;
    } else {
      stat_result = os_stat(os_path, &stat_buf);
      if (stat_result == 0 && S_ISREG(stat_buf.st_mode)) {
        if (sizeof(stat_buf.st_size) == 8) {
          result = bigFromUInt64((uint64type) stat_buf.st_size);
        } else {
          result = bigFromUInt32((uint32type) stat_buf.st_size);
        } /* if */
      } else if (stat_result == 0 && S_ISDIR(stat_buf.st_mode)) {
        result = bigIConv(0);
      } else {
#ifdef OS_PATH_WCHAR
        aFile = wide_fopen(os_path, L"r");
#else
        aFile = fopen(os_path, "r");
#endif
        if (aFile == NULL) {
          err_info = FILE_ERROR;
          result = NULL;
        } else {
          result = getBigFileLengthUsingSeek(aFile);
          fclose(aFile);
        } /* if */
      } /* if */
      os_path_free(os_path);
    } /* if */
    if (err_info != OKAY_NO_ERROR) {
      raise_error(err_info);
    } /* if */
    return(result);
  } /* cmdBigFileSize */



#ifdef ANSI_C

void cmdChdir (stritype dir_name)
#else

void cmdChdir (dir_name)
stritype dir_name;
#endif

  {
    os_path_stri os_path;
    errinfotype err_info = OKAY_NO_ERROR;
    int chdir_result;

  /* cmdChdir */
    os_path = cp_to_os_path(dir_name, &err_info);
    if (os_path == NULL) {
      raise_error(err_info);
    } else {
      chdir_result = os_chdir(os_path);
      os_path_free(os_path);
      if (chdir_result != 0) {
        raise_error(FILE_ERROR);
      } /* if */
    } /* if */
  } /* cmdChdir */



#ifdef ANSI_C

void cmdCloneFile (stritype source_name, stritype dest_name)
#else

void cmdCloneFile (source_name, dest_name)
stritype source_name;
stritype dest_name;
#endif

  {
    os_path_stri os_source_name;
    os_path_stri os_dest_name;
    errinfotype err_info = OKAY_NO_ERROR;

  /* cmdCloneFile */
    os_source_name = cp_to_os_path(source_name, &err_info);
    if (os_source_name != NULL) {
      os_dest_name = cp_to_os_path(dest_name, &err_info);
      if (os_dest_name != NULL) {
        copy_any_file(os_source_name, os_dest_name, PRESERVE_ALL, &err_info);
        os_path_free(os_dest_name);
      } /* if */
      os_path_free(os_source_name);
    } /* if */
    if (err_info != OKAY_NO_ERROR) {
      raise_error(err_info);
    } /* if */
  } /* cmdCloneFile */



#ifdef ANSI_C

stritype cmdConfigValue (stritype name)
#else

stritype cmdConfigValue (name)
stritype name;
#endif

  {
    char opt_name[250];
    cstritype opt;
    stritype result;

  /* cmdConfigValue */
    if (compr_size(name) + 1 > 250) {
      opt = "";
    } else {
      stri_export((ustritype) opt_name, name);
      if (strcmp(opt_name, "OBJECT_FILE_EXTENSION") == 0) {
        opt = OBJECT_FILE_EXTENSION;
      } else if (strcmp(opt_name, "EXECUTABLE_FILE_EXTENSION") == 0) {
        opt = EXECUTABLE_FILE_EXTENSION;
      } else if (strcmp(opt_name, "C_COMPILER") == 0) {
        opt = C_COMPILER;
      } else if (strcmp(opt_name, "INHIBIT_C_WARNINGS") == 0) {
        opt = INHIBIT_C_WARNINGS;
      } else if (strcmp(opt_name, "REDIRECT_C_ERRORS") == 0) {
        opt = REDIRECT_C_ERRORS;
      } else if (strcmp(opt_name, "LINKER_FLAGS") == 0) {
        opt = LINKER_FLAGS;
      } else if (strcmp(opt_name, "SYSTEM_LIBS") == 0) {
        opt = SYSTEM_LIBS;
      } else if (strcmp(opt_name, "SEED7_LIB") == 0) {
        opt = SEED7_LIB;
      } else if (strcmp(opt_name, "COMP_DATA_LIB") == 0) {
        opt = COMP_DATA_LIB;
      } else if (strcmp(opt_name, "COMPILER_LIB") == 0) {
        opt = COMPILER_LIB;
      } else if (strcmp(opt_name, "INTTYPE_LITERAL_SUFFIX") == 0) {
        opt = INTTYPE_LITERAL_SUFFIX;
      } else if (strcmp(opt_name, "USE_SIGSETJMP") == 0) {
#ifdef USE_SIGSETJMP
        opt = "TRUE";
#else
        opt = "FALSE";
#endif
      } else if (strcmp(opt_name, "ISNAN_WITH_UNDERLINE") == 0) {
#ifdef ISNAN_WITH_UNDERLINE
        opt = "TRUE";
#else
        opt = "FALSE";
#endif
      } else if (strcmp(opt_name, "CHECK_INT_DIV_BY_ZERO") == 0) {
#ifdef CHECK_INT_DIV_BY_ZERO
        opt = "TRUE";
#else
        opt = "FALSE";
#endif
      } else if (strcmp(opt_name, "WITH_STRI_CAPACITY") == 0) {
#ifdef WITH_STRI_CAPACITY
        opt = "TRUE";
#else
        opt = "FALSE";
#endif
      } else if (strcmp(opt_name, "RSHIFT_DOES_SIGN_EXTEND") == 0) {
#ifdef RSHIFT_DOES_SIGN_EXTEND
        opt = "TRUE";
#else
        opt = "FALSE";
#endif
      } else if (strcmp(opt_name, "TWOS_COMPLEMENT_INTTYPE") == 0) {
#ifdef TWOS_COMPLEMENT_INTTYPE
        opt = "TRUE";
#else
        opt = "FALSE";
#endif
      } else if (strcmp(opt_name, "FLOAT_ZERO_DIV_ERROR") == 0) {
#ifdef FLOAT_ZERO_DIV_ERROR
        opt = "TRUE";
#else
        opt = "FALSE";
#endif
      } else if (strcmp(opt_name, "INTTYPE_64BIT") == 0) {
#ifdef INTTYPE_64BIT
        opt = "TRUE";
#else
        opt = "FALSE";
#endif
      } else {
        opt = "";
      } /* if */
    } /* if */
    result = cstri_to_stri(opt);
    if (result == NULL) {
      raise_error(MEMORY_ERROR);
    } /* if */
    return(result);
  } /* cmdConfigValue */



#ifdef ANSI_C

void cmdCopyFile (stritype source_name, stritype dest_name)
#else

void cmdCopyFile (source_name, dest_name)
stritype source_name;
stritype dest_name;
#endif

  {
    os_path_stri os_source_name;
    os_path_stri os_dest_name;
    errinfotype err_info = OKAY_NO_ERROR;

  /* cmdCopyFile */
    os_source_name = cp_to_os_path(source_name, &err_info);
    if (os_source_name != NULL) {
      os_dest_name = cp_to_os_path(dest_name, &err_info);
      if (os_dest_name != NULL) {
        copy_any_file(os_source_name, os_dest_name, PRESERVE_NOTHING, &err_info);
        os_path_free(os_dest_name);
      } /* if */
      os_path_free(os_source_name);
    } /* if */
    if (err_info != OKAY_NO_ERROR) {
      raise_error(err_info);
    } /* if */
  } /* cmdCopyFile */



#ifdef ANSI_C

inttype cmdFileSize (stritype file_name)
#else

inttype cmdFileSize (file_name)
stritype file_name;
#endif

  {
    os_path_stri os_path;
    os_stat_struct stat_buf;
    int stat_result;
    filetype aFile;
    errinfotype err_info = OKAY_NO_ERROR;
    inttype result;

  /* cmdFileSize */
    os_path = cp_to_os_path(file_name, &err_info);
    if (os_path == NULL) {
      result = 0;
    } else {
      stat_result = os_stat(os_path, &stat_buf);
      if (stat_result == 0 && S_ISREG(stat_buf.st_mode)) {
        if (stat_buf.st_size > MAX_INTEGER || stat_buf.st_size < 0) {
          err_info = RANGE_ERROR;
          result = 0;
        } else {
          result = (inttype) stat_buf.st_size;
        } /* if */
      } else if (stat_result == 0 && S_ISDIR(stat_buf.st_mode)) {
        result = 0;
      } else {
#ifdef OS_PATH_WCHAR
        aFile = wide_fopen(os_path, L"r");
#else
        aFile = fopen(os_path, "r");
#endif
        if (aFile == NULL) {
          err_info = FILE_ERROR;
          result = 0;
        } else {
          result = getFileLengthUsingSeek(aFile);
          fclose(aFile);
        } /* if */
      } /* if */
      os_path_free(os_path);
    } /* if */
    if (err_info != OKAY_NO_ERROR) {
      raise_error(err_info);
    } /* if */
    return(result);
  } /* cmdFileSize */



#ifdef ANSI_C

inttype cmdFileType (stritype file_name)
#else

inttype cmdFileType (file_name)
stritype file_name;
#endif

  {
    os_path_stri os_path;
    os_stat_struct stat_buf;
    int stat_result;
    errinfotype err_info = OKAY_NO_ERROR;
    inttype result;

  /* cmdFileType */
    os_path = cp_to_os_path(file_name, &err_info);
    if (os_path == NULL) {
      raise_error(err_info);
      result = 0;
    } else {
      stat_result = os_stat(os_path, &stat_buf);
      os_path_free(os_path);
      if (stat_result == 0) {
        if (S_ISREG(stat_buf.st_mode)) {
          result = 2;
        } else if (S_ISDIR(stat_buf.st_mode)) {
          result = 3;
        } else if (S_ISCHR(stat_buf.st_mode)) {
          result = 4;
        } else if (S_ISBLK(stat_buf.st_mode)) {
          result = 5;
        } else if (S_ISFIFO(stat_buf.st_mode)) {
          result = 6;
        } else if (S_ISLNK(stat_buf.st_mode)) {
          result = 7;
        } else if (S_ISSOCK(stat_buf.st_mode)) {
          result = 8;
        } else {
          result = 1;
        } /* if */
      } else {
        result = 0;
        if (errno != ENOENT || file_name->size == 0) {
          raise_error(FILE_ERROR);
        } /* if */
      } /* if */
    } /* if */
    return(result);
  } /* cmdFileType */



#ifdef ANSI_C

inttype cmdFileTypeSL (stritype file_name)
#else

inttype cmdFileTypeSL (file_name)
stritype file_name;
#endif

  {
    os_path_stri os_path;
    os_stat_struct stat_buf;
    int stat_result;
    errinfotype err_info = OKAY_NO_ERROR;
    inttype result;

  /* cmdFileTypeSL */
    os_path = cp_to_os_path(file_name, &err_info);
    if (os_path == NULL) {
      raise_error(err_info);
      result = 0;
    } else {
      stat_result = os_lstat(os_path, &stat_buf);
      os_path_free(os_path);
      if (stat_result == 0) {
        if (S_ISREG(stat_buf.st_mode)) {
          result = 2;
        } else if (S_ISDIR(stat_buf.st_mode)) {
          result = 3;
        } else if (S_ISCHR(stat_buf.st_mode)) {
          result = 4;
        } else if (S_ISBLK(stat_buf.st_mode)) {
          result = 5;
        } else if (S_ISFIFO(stat_buf.st_mode)) {
          result = 6;
        } else if (S_ISLNK(stat_buf.st_mode)) {
          result = 7;
        } else if (S_ISSOCK(stat_buf.st_mode)) {
          result = 8;
        } else {
          result = 1;
        } /* if */
      } else {
        result = 0;
        if (errno != ENOENT || file_name->size == 0) {
          raise_error(FILE_ERROR);
        } /* if */
      } /* if */
    } /* if */
    return(result);
  } /* cmdFileTypeSL */



#ifdef ANSI_C

stritype cmdGetcwd (void)
#else

stritype cmdGetcwd ()
#endif

  {
    os_path_char buffer[PATH_MAX + 1];
    os_path_stri cwd;
    stritype result;

  /* cmdGetcwd */
    if ((cwd = os_getcwd(buffer, PATH_MAX)) == NULL) {
      raise_error(MEMORY_ERROR);
      result = NULL;
    } else {
#ifdef OS_PATH_WCHAR
      result = wstri_to_stri(cwd);
#else
#ifdef OS_PATH_UTF8
      result = cstri8_to_stri(cwd);
      if (result == NULL) {
        result = cstri_to_stri(cwd);
      } /* if */
#else
      result = cstri_to_stri(cwd);
#endif
#endif
      if (result == NULL) {
        raise_error(MEMORY_ERROR);
#if PATH_DELIMITER != '/'
      } else {
        unsigned int pos;

        for (pos = 0; pos < result->size; pos++) {
          if (result->mem[pos] == PATH_DELIMITER) {
            result->mem[pos] = (strelemtype) '/';
          } /* if */
        } /* for */
#endif
      } /* if */
    } /* if */
    return(result);
  } /* cmdGetcwd */



#ifdef ANSI_C

void cmdGetATime (stritype file_name,
    inttype *year, inttype *month, inttype *day, inttype *hour,
    inttype *min, inttype *sec, inttype *mycro_sec, inttype *time_zone,
    booltype *is_dst)
#else

void cmdGetATime (file_name,
    year, month, day, hour, min, sec, mycro_sec, time_zone, is_dst)
stritype file_name;
inttype *year;
inttype *month;
inttype *day;
inttype *hour;
inttype *min;
inttype *sec;
inttype *mycro_sec;
inttype *time_zone;
booltype *is_dst;
#endif

  {
    os_path_stri os_path;
    os_stat_struct stat_buf;
    int stat_result;
    errinfotype err_info = OKAY_NO_ERROR;

  /* cmdGetATime */
#ifdef TRACE_CMD_RTL
    printf("BEGIN cmdGetATime\n");
#endif
    os_path = cp_to_os_path(file_name, &err_info);
    if (os_path == NULL) {
      raise_error(err_info);
    } else {
      stat_result = os_stat(os_path, &stat_buf);
      os_path_free(os_path);
      if (stat_result == 0) {
        timFromTimestamp(stat_buf.st_atime,
            year, month, day, hour,
	    min, sec, mycro_sec, time_zone, is_dst);
      } else {
        raise_error(FILE_ERROR);
      } /* if */
    } /* if */
#ifdef TRACE_CMD_RTL
    printf("END cmdGetATime(%04ld-%02ld-%02ld %02ld:%02ld:%02ld.%06ld %ld %d)\n",
        *year, *month, *day, *hour, *min, *sec,
        *mycro_sec, *time_zone, *is_dst);
#endif
  } /* cmdGetATime */



#ifdef ANSI_C

void cmdGetCTime (stritype file_name,
    inttype *year, inttype *month, inttype *day, inttype *hour,
    inttype *min, inttype *sec, inttype *mycro_sec, inttype *time_zone,
    booltype *is_dst)
#else

void cmdGetCTime (file_name,
    year, month, day, hour, min, sec, mycro_sec, time_zone, is_dst)
stritype file_name;
inttype *year;
inttype *month;
inttype *day;
inttype *hour;
inttype *min;
inttype *sec;
inttype *mycro_sec;
inttype *time_zone;
booltype *is_dst;
#endif

  {
    os_path_stri os_path;
    os_stat_struct stat_buf;
    int stat_result;
    errinfotype err_info = OKAY_NO_ERROR;

  /* cmdGetCTime */
#ifdef TRACE_CMD_RTL
    printf("BEGIN cmdGetCTime\n");
#endif
    os_path = cp_to_os_path(file_name, &err_info);
    if (os_path == NULL) {
      raise_error(err_info);
    } else {
      stat_result = os_stat(os_path, &stat_buf);
      os_path_free(os_path);
      if (stat_result == 0) {
        timFromTimestamp(stat_buf.st_ctime,
            year, month, day, hour,
	    min, sec, mycro_sec, time_zone, is_dst);
      } else {
        raise_error(FILE_ERROR);
      } /* if */
    } /* if */
#ifdef TRACE_CMD_RTL
    printf("END cmdGetCTime(%04ld-%02ld-%02ld %02ld:%02ld:%02ld.%06ld %ld %d)\n",
        *year, *month, *day, *hour, *min, *sec,
        *mycro_sec, *time_zone, *is_dst);
#endif
  } /* cmdGetCTime */



#ifdef ANSI_C

void cmdGetMTime (stritype file_name,
    inttype *year, inttype *month, inttype *day, inttype *hour,
    inttype *min, inttype *sec, inttype *mycro_sec, inttype *time_zone,
    booltype *is_dst)
#else

void cmdGetMTime (file_name,
    year, month, day, hour, min, sec, mycro_sec, time_zone, is_dst)
stritype file_name;
inttype *year;
inttype *month;
inttype *day;
inttype *hour;
inttype *min;
inttype *sec;
inttype *mycro_sec;
inttype *time_zone;
booltype *is_dst;
#endif

  {
    os_path_stri os_path;
    os_stat_struct stat_buf;
    int stat_result;
    errinfotype err_info = OKAY_NO_ERROR;

  /* cmdGetMTime */
#ifdef TRACE_CMD_RTL
    printf("BEGIN cmdGetMTime(");
    prot_stri(file_name);
    printf(")\n");
#endif
    os_path = cp_to_os_path(file_name, &err_info);
    if (os_path == NULL) {
      raise_error(err_info);
    } else {
      stat_result = os_stat(os_path, &stat_buf);
      os_path_free(os_path);
      if (stat_result == 0) {
        timFromTimestamp(stat_buf.st_mtime,
            year, month, day, hour,
	    min, sec, mycro_sec, time_zone, is_dst);
      } else {
        /* printf("errno=%d\n", errno); */
        raise_error(FILE_ERROR);
      } /* if */
    } /* if */
#ifdef TRACE_CMD_RTL
    printf("END cmdGetMTime(%04ld-%02ld-%02ld %02ld:%02ld:%02ld.%06ld %ld %d)\n",
        *year, *month, *day, *hour, *min, *sec,
        *mycro_sec, *time_zone, *is_dst);
#endif
  } /* cmdGetMTime */



#ifdef ANSI_C

rtlArraytype cmdLs (stritype dir_name)
#else

rtlArraytype cmdLs (dir_name)
stritype dir_name;
#endif

  {
    errinfotype err_info = OKAY_NO_ERROR;
    rtlArraytype result;

  /* cmdLs */
    result = read_dir(dir_name, &err_info);
    if (result == NULL) {
      raise_error(err_info);
    } else {
      qsort((void *) result->arr,
          (size_t) (result->max_position - result->min_position + 1),
          sizeof(rtlObjecttype), &cmp_mem);
    } /* if */
    return(result);
  } /* cmdLs */



#ifdef ANSI_C

void cmdMkdir (stritype dir_name)
#else

void cmdMkdir (dir_name)
stritype dir_name;
#endif

  {
    os_path_stri os_path;
    int mkdir_result;
    errinfotype err_info = OKAY_NO_ERROR;

  /* cmdMkdir */
    os_path = cp_to_os_path(dir_name, &err_info);
    if (os_path == NULL) {
      raise_error(err_info);
    } else {
      mkdir_result = os_mkdir(os_path, 0777);
      os_path_free(os_path);
      if (mkdir_result != 0) {
        raise_error(FILE_ERROR);
      } /* if */
    } /* if */
  } /* cmdMkdir */



#ifdef ANSI_C

void cmdMove (stritype source_name, stritype dest_name)
#else

void cmdMove (source_name, dest_name)
stritype source_name;
stritype dest_name;
#endif

  {
    os_path_stri os_source_name;
    os_path_stri os_dest_name;
    errinfotype err_info = OKAY_NO_ERROR;

  /* cmdMove */
    os_source_name = cp_to_os_path(source_name, &err_info);
    if (os_source_name != NULL) {
      os_dest_name = cp_to_os_path(dest_name, &err_info);
      if (os_dest_name != NULL) {
        move_any_file(os_source_name, os_dest_name, &err_info);
        os_path_free(os_dest_name);
      } /* if */
      os_path_free(os_source_name);
    } /* if */
    if (err_info != OKAY_NO_ERROR) {
      raise_error(err_info);
    } /* if */
  } /* cmdMove */



#ifdef ANSI_C

stritype cmdReadlink (stritype link_name)
#else

stritype cmdReadlink (link_name)
stritype link_name;
#endif

  {
#ifdef HAS_SYMLINKS
    os_path_stri os_link_name;
    os_stat_struct link_stat;
    os_path_stri link_destination;
    ssize_t readlink_result;
#endif
    errinfotype err_info = OKAY_NO_ERROR;
    stritype result = NULL;

  /* cmdReadlink */
#ifdef HAS_SYMLINKS
    os_link_name = cp_to_os_path(link_name, &err_info);
    if (os_link_name != NULL) {
      if (os_lstat(os_link_name, &link_stat) != 0 || !S_ISLNK(link_stat.st_mode)) {
        err_info = FILE_ERROR;
      } else {
        /* printf("link size=%lu\n", link_stat.st_size); */
        if (link_stat.st_size > MAX_SYMLINK_PATH_LENGTH || link_stat.st_size < 0) {
          err_info = RANGE_ERROR;
        } else {
          if (!os_path_alloc(link_destination, link_stat.st_size)) {
            err_info = MEMORY_ERROR;
          } else {
            readlink_result = readlink(os_link_name, link_destination,
                                       (size_t) link_stat.st_size);
            if (readlink_result != -1) {
              link_destination[readlink_result] = '\0';
#ifdef OS_PATH_WCHAR
              result = wstri_to_stri(link_destination);
#else
#ifdef OS_PATH_UTF8
              result = cstri8_to_stri(link_destination);
              if (result == NULL) {
                result = cstri_to_stri(link_destination);
              } /* if */
#else
              result = cstri_to_stri(link_destination);
#endif
#endif
              if (result == NULL) {
                err_info = MEMORY_ERROR;
              } /* if */
            } else {
              err_info = FILE_ERROR;
            } /* if */
            os_path_free(link_destination);
          } /* if */
        } /* if */
      } /* if */
      os_path_free(os_link_name);
    } /* if */
#else
    err_info = FILE_ERROR;
#endif
    if (err_info != OKAY_NO_ERROR) {
      raise_error(err_info);
    } /* if */
    return(result);
  } /* cmdReadlink */



#ifdef ANSI_C

void cmdRemove (stritype file_name)
#else

void cmdRemove (file_name)
stritype file_name;
#endif

  {
    os_path_stri os_file_name;
    errinfotype err_info = OKAY_NO_ERROR;

  /* cmdRemove */
    os_file_name = cp_to_os_path(file_name, &err_info);
    if (os_file_name == NULL) {
      raise_error(err_info);
    } else {
      os_remove(os_file_name);
      os_path_free(os_file_name);
    } /* if */
  } /* cmdRemove */



#ifdef ANSI_C

void cmdRemoveAnyFile (stritype file_name)
#else

void cmdRemoveAnyFile (file_name)
stritype file_name;
#endif

  {
    os_path_stri os_file_name;
    errinfotype err_info = OKAY_NO_ERROR;

  /* cmdRemoveAnyFile */
#ifdef TRACE_CMD_RTL
    printf("BEGIN cmdRemoveAnyFile\n");
#endif
    os_file_name = cp_to_os_path(file_name, &err_info);
    if (os_file_name != NULL) {
      remove_any_file(os_file_name, &err_info);
      os_path_free(os_file_name);
    } /* if */
    if (err_info != OKAY_NO_ERROR) {
      raise_error(err_info);
    } /* if */
#ifdef TRACE_CMD_RTL
    printf("END cmdRemoveAnyFile\n");
#endif
  } /* cmdRemoveAnyFile */



#ifdef ANSI_C

void cmdSetATime (stritype file_name,
    inttype year, inttype month, inttype day, inttype hour,
    inttype min, inttype sec, inttype mycro_sec, inttype time_zone)
#else

void cmdSetATime (file_name,
    year, month, day, hour, min, sec, mycro_sec, time_zone)
 stritype file_name;
inttype year;
inttype month;
inttype day;
inttype hour;
inttype min;
inttype sec;
inttype mycro_sec;
inttype time_zone;
#endif

  {
    os_path_stri os_path;
    os_stat_struct stat_buf;
    os_utimbuf_struct utime_buf;
    errinfotype err_info = OKAY_NO_ERROR;

  /* cmdSetATime */
    os_path = cp_to_os_path(file_name, &err_info);
    if (os_path != NULL) {
      if (os_stat(os_path, &stat_buf) == 0) {
        utime_buf.actime = timToTimestamp(year, month, day, hour,
	    min, sec, mycro_sec, time_zone);
        utime_buf.modtime = stat_buf.st_mtime;
        if (os_utime(os_path, &utime_buf) != 0) {
          err_info = FILE_ERROR;
        } /* if */
      } else {
        err_info = FILE_ERROR;
      } /* if */
      os_path_free(os_path);
    } /* if */
    if (err_info != OKAY_NO_ERROR) {
      raise_error(err_info);
    } /* if */
  } /* cmdSetATime */



#ifdef ANSI_C

void cmdSetMTime (stritype file_name,
    inttype year, inttype month, inttype day, inttype hour,
    inttype min, inttype sec, inttype mycro_sec, inttype time_zone)
#else

void cmdSetMTime (file_name,
    year, month, day, hour, min, sec, mycro_sec, time_zone)
 stritype file_name;
inttype year;
inttype month;
inttype day;
inttype hour;
inttype min;
inttype sec;
inttype mycro_sec;
inttype time_zone;
#endif

  {
    os_path_stri os_path;
    os_stat_struct stat_buf;
    os_utimbuf_struct utime_buf;
    errinfotype err_info = OKAY_NO_ERROR;

  /* cmdSetMTime */
    os_path = cp_to_os_path(file_name, &err_info);
    if (os_path != NULL) {
      if (os_stat(os_path, &stat_buf) == 0) {
        utime_buf.actime = stat_buf.st_atime;
        utime_buf.modtime = timToTimestamp(year, month, day, hour,
	    min, sec, mycro_sec, time_zone);
        if (os_utime(os_path, &utime_buf) != 0) {
          err_info = FILE_ERROR;
        } /* if */
      } else {
        err_info = FILE_ERROR;
      } /* if */
      os_path_free(os_path);
    } /* if */
    if (err_info != OKAY_NO_ERROR) {
      raise_error(err_info);
    } /* if */
  } /* cmdSetMTime */



#ifdef ANSI_C

inttype cmdShell (stritype command_stri)
#else

inttype cmdShell (command_stri)
stritype command_stri;
#endif

  {
    cstritype os_command_stri;
    errinfotype err_info = OKAY_NO_ERROR;
    inttype result;

  /* cmdShell */
    os_command_stri = cp_to_command(command_stri, &err_info);
    if (os_command_stri == NULL) {
      raise_error(err_info);
      result = 0;
    } else {
      result = (inttype) system(os_command_stri);
      free_cstri(os_command_stri, command_stri);
    } /* if */
    return(result);
  } /* cmdShell */



#ifdef ANSI_C

void cmdSymlink (stritype source_name, stritype dest_name)
#else

void cmdSymlink (source_name, dest_name)
stritype source_name;
stritype dest_name;
#endif

  {
#ifdef HAS_SYMLINKS
    os_path_stri os_source_name;
    os_path_stri os_dest_name;
#endif
    errinfotype err_info = OKAY_NO_ERROR;

  /* cmdSymlink */
#ifdef HAS_SYMLINKS
    os_source_name = cp_to_os_path(source_name, &err_info);
    if (os_source_name != NULL) {
      os_dest_name = cp_to_os_path(dest_name, &err_info);
      if (os_dest_name != NULL) {
        if (symlink(os_source_name, os_dest_name) != 0) {
          err_info = FILE_ERROR;
        } /* if */
        os_path_free(os_dest_name);
      } /* if */
      os_path_free(os_source_name);
    } /* if */
#else
    err_info = FILE_ERROR;
#endif
    if (err_info != OKAY_NO_ERROR) {
      raise_error(err_info);
    } /* if */
  } /* cmdSymlink */



#ifdef FTELL_WRONG_FOR_PIPE
#undef ftell



#ifdef ANSI_C

long improved_ftell (FILE *stream)
#else

long improved_ftell (stream)
FILE *stream;
#endif

  {
    int file_no;
    os_stat_struct stat_buf;
    int result;

  /* improved_ftell */
    file_no = fileno(stream);
    if (file_no != -1 && os_fstat(file_no, &stat_buf) == 0 &&
        S_ISREG(stat_buf.st_mode)) {
      result = ftell(stream);
    } else {
      result = -1;
    } /* if */
    return(result);
  } /* improved_ftell */
#endif
