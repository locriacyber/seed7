/********************************************************************/
/*                                                                  */
/*  s7   Seed7 interpreter                                          */
/*  Copyright (C) 1990 - 2015  Thomas Mertes                        */
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
/*  License along with this program; if not, write to the           */
/*  Free Software Foundation, Inc., 51 Franklin Street,             */
/*  Fifth Floor, Boston, MA  02110-1301, USA.                       */
/*                                                                  */
/*  Module: Compiler data reflection                                */
/*  File: seed7/src/ref_data.c                                      */
/*  Changes: 1991-1994, 2004, 2007, 2008, 2010, 2015  Thomas Mertes */
/*  Content: Primitive actions for the reference type.              */
/*                                                                  */
/********************************************************************/

#define LOG_FUNCTIONS 0
#define VERBOSE_EXCEPTIONS 0

#include "version.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "common.h"
#include "data.h"
#include "data_rtl.h"
#include "heaputl.h"
#include "flistutl.h"
#include "datautl.h"
#include "striutl.h"
#include "listutl.h"
#include "identutl.h"
#include "objutl.h"
#include "infile.h"    /* from the compiler library */
#include "name.h"      /* from the compiler library */
#include "blockutl.h"  /* from the compiler library */
#include "syvarutl.h"  /* from the compiler library */
#include "hsh_rtl.h"
#include "int_rtl.h"
#include "str_rtl.h"
#include "big_drv.h"
#include "pol_drv.h"
#include "rtl_err.h"

#undef EXTERN
#define EXTERN
#include "ref_data.h"


#define TABLE_INCREMENT 127



objectType refAlloc (const const_objectType obj_arg)

  {
    objectType created_object;

  /* refAlloc */
    logFunction(printf("refAlloc(");
                trace1(obj_arg);
                printf(")\n"););
    if (unlikely(!ALLOC_OBJECT(created_object))) {
      raise_error(MEMORY_ERROR);
    } else {
      created_object->type_of = obj_arg->type_of;
      memcpy(&created_object->descriptor, &obj_arg->descriptor,
          sizeof(descriptorUnion));
      /* Copies the POSINFO flag (and all other flags): */
      INIT_CATEGORY_OF_OBJ(created_object, obj_arg->objcategory);
      created_object->value.objValue = NULL;
    } /* if */
    return created_object;
  } /* refAlloc */



objectType refAllocStri (boolType var_flag, typeType any_type,
    const const_striType stri_from)

  {
    objectType created_object;

  /* refAllocStri */
    if (unlikely(!ALLOC_OBJECT(created_object))) {
      raise_error(MEMORY_ERROR);
    } else {
      created_object->type_of = any_type;
      created_object->descriptor.property = NULL;
      INIT_CATEGORY_OF_OBJ(created_object, STRIOBJECT);
      if (var_flag) {
        SET_VAR_FLAG(created_object);
      } /* if */
      created_object->value.striValue = strCreate(stri_from);
    } /* if */
    return created_object;
  } /* refAllocStri */



intType refArrMaxIdx (const const_objectType obj_arg)

  { /* refArrMaxIdx */
    if (unlikely(obj_arg == NULL ||
                 CATEGORY_OF_OBJ(obj_arg) != ARRAYOBJECT)) {
      logError(printf("refArrMaxIdx(");
               trace1(obj_arg);
               printf("): Category is not ARRAYOBJECT.\n"););
      raise_error(RANGE_ERROR);
      return 0;
    } else {
      return take_array(obj_arg)->max_position;
    } /* if */
  } /* refArrMaxIdx */



intType refArrMinIdx (const const_objectType obj_arg)

  { /* refArrMinIdx */
    if (unlikely(obj_arg == NULL ||
                 CATEGORY_OF_OBJ(obj_arg) != ARRAYOBJECT)) {
      logError(printf("refArrMinIdx(");
               trace1(obj_arg);
               printf("): Category is not ARRAYOBJECT.\n"););
      raise_error(RANGE_ERROR);
      return 0;
    } else {
      return take_array(obj_arg)->min_position;
    } /* if */
  } /* refArrMinIdx */



listType refArrToList (const const_objectType obj_arg)

  {
    errInfoType err_info = OKAY_NO_ERROR;
    listType result;

  /* refArrToList */
    if (unlikely(obj_arg == NULL ||
                 CATEGORY_OF_OBJ(obj_arg) != ARRAYOBJECT)) {
      logError(printf("refArrToList(");
               trace1(obj_arg);
               printf("): Category is not ARRAYOBJECT.\n"););
      raise_error(RANGE_ERROR);
      result = NULL;
    } else {
      result = array_to_list(take_array(obj_arg), &err_info);
      if (unlikely(err_info != OKAY_NO_ERROR)) {
        raise_error(MEMORY_ERROR);
        result = NULL;
      } /* if */
    } /* if */
    return result;
  } /* refArrToList */



objectType refBody (const const_objectType obj_arg)

  {
    objectType result;

  /* refBody */
    if (unlikely(obj_arg == NULL ||
                 CATEGORY_OF_OBJ(obj_arg) != BLOCKOBJECT)) {
      logError(printf("refBody(");
               trace1(obj_arg);
               printf("): Category is not BLOCKOBJECT.\n"););
      raise_error(RANGE_ERROR);
      result = NULL;
    } else {
      result = obj_arg->value.blockValue->body;
    } /* if */
    return result;
  } /* refBody */



intType refCategory (const const_objectType obj_arg)

  {
    intType result;

  /* refCategory */
    if (unlikely(obj_arg == NULL)) {
      logError(printf("refCategory(NULL): Object is NULL.\n"););
      raise_error(RANGE_ERROR);
      result = 0;
    } else {
      result = CATEGORY_OF_OBJ(obj_arg);
    } /* if */
    return result;
  } /* refCategory */



intType refCatParse (striType category_name)

  {
    cstriType name;
    errInfoType err_info = OKAY_NO_ERROR;
    intType result;

  /* refCatParse */
    name = stri_to_cstri(category_name, &err_info);
    if (unlikely(name == NULL)) {
      logError(printf("refCatParse: stri_to_cstri(\"%s\", *) failed:\n"
                      "err_info=%d\n",
                      striAsUnquotedCStri(category_name),
                      err_info););
      raise_error(err_info);
      result = -1;
    } else {
      result = category_value(name);
      free_cstri(name, category_name);
      if (unlikely(result == -1)) {
        logError(printf("refCatParse: category_value(\"%s\") failed.\n",
                        striAsUnquotedCStri(category_name)););
        raise_error(RANGE_ERROR);
      } /* if */
    } /* if */
    return result;
  } /* refCatParse */



striType refCatStr (intType aCategory)

  {
    striType result;

  /* refCatStr */
    result = cstri_to_stri(category_cstri((objectCategory) aCategory));
    if (unlikely(result == NULL)) {
      raise_error(MEMORY_ERROR);
    } /* if */
    return result;
  } /* refCatStr */



striType refFile (const const_objectType obj_arg)

  {
    fileNumType file_number;
    striType result;

  /* refFile */
    if (unlikely(obj_arg == NULL)) {
      logError(printf("refFile(NULL): Object is NULL.\n"););
      raise_error(RANGE_ERROR);
      result = NULL;
    } else {
      if (HAS_POSINFO(obj_arg)) {
        file_number = GET_FILE_NUM(obj_arg);
      } else if (HAS_PROPERTY(obj_arg)) {
        /* trace1(obj_arg);
        printf(" %u %u %u\n",
            obj_arg->descriptor.property->file_number,
            obj_arg->descriptor.property->line,
            obj_arg->descriptor.property->syNumberInLine); */
        file_number = obj_arg->descriptor.property->file_number;
      } else {
        file_number = 0;
      } /* if */
      result = get_file_name(file_number);
      if (unlikely(result == NULL)) {
        raise_error(MEMORY_ERROR);
      } /* if */
    } /* if */
    return result;
  } /* refFile */



listType refHshDataToList (const const_objectType obj_arg)

  {
    errInfoType err_info = OKAY_NO_ERROR;
    listType result;

  /* refHshDataToList */
    if (unlikely(obj_arg == NULL ||
                 CATEGORY_OF_OBJ(obj_arg) != HASHOBJECT)) {
      logError(printf("refHshDataToList(");
               trace1(obj_arg);
               printf("): Category is not HASHOBJECT.\n"););
      raise_error(RANGE_ERROR);
      result = NULL;
    } else {
      result = hash_data_to_list(take_hash(obj_arg), &err_info);
      if (unlikely(err_info != OKAY_NO_ERROR)) {
        raise_error(MEMORY_ERROR);
        result = NULL;
      } /* if */
    } /* if */
    return result;
  } /* refHshDataToList */



listType refHshKeysToList (const const_objectType obj_arg)

  {
    errInfoType err_info = OKAY_NO_ERROR;
    listType result;

  /* refHshKeysToList */
    if (unlikely(obj_arg == NULL ||
                 CATEGORY_OF_OBJ(obj_arg) != HASHOBJECT)) {
      logError(printf("refHshKeysToList(");
               trace1(obj_arg);
               printf("): Category is not HASHOBJECT.\n"););
      raise_error(RANGE_ERROR);
      result = NULL;
    } else {
      result = hash_keys_to_list(take_hash(obj_arg), &err_info);
      if (unlikely(err_info != OKAY_NO_ERROR)) {
        raise_error(MEMORY_ERROR);
        result = NULL;
      } /* if */
    } /* if */
    return result;
  } /* refHshKeysToList */



boolType refIsVar (const const_objectType obj_arg)

  { /* refIsVar */
    logFunction(printf("refIsVar(" FMT_U_MEM ")\n", (memSizeType) obj_arg););
    if (unlikely(obj_arg == NULL)) {
      logError(printf("refIsVar(NULL): Object is NULL.\n"););
      raise_error(RANGE_ERROR);
      return FALSE;
    } else {
      return VAR_OBJECT(obj_arg) ? TRUE : FALSE;
    } /* if */
  } /* refIsVar */



objectType refItfToSct (const const_objectType obj_arg)

  {
    objectType result;

  /* refItfToSct */
    if (unlikely(obj_arg == NULL ||
                 CATEGORY_OF_OBJ(obj_arg) != INTERFACEOBJECT ||
                 take_reference(obj_arg) == NULL)) {
      logError(printf("refItfToSct(");
               trace1(obj_arg);
               printf("): Category is not INTERFACEOBJECT.\n"););
      raise_error(RANGE_ERROR);
      result = NULL;
    } else {
      result = take_reference(obj_arg);
    } /* if */
    return result;
  } /* refItfToSct */



intType refLine (const const_objectType obj_arg)

  {
    intType result;

  /* refLine */
    if (unlikely(obj_arg == NULL)) {
      logError(printf("refLine(NULL): Object is NULL.\n"););
      raise_error(RANGE_ERROR);
      result = 0;
    } else if (HAS_POSINFO(obj_arg)) {
      /* GET_LINE_NUM delivers an unsigned integer in the range 0 to 1048575 */
      result = (intType) GET_LINE_NUM(obj_arg);
    } else if (HAS_PROPERTY(obj_arg)) {
      /* trace1(obj_arg);
      printf(" %s %u %u\n",
          get_file_name_ustri(obj_arg->descriptor.property->file_number),
          obj_arg->descriptor.property->line,
          obj_arg->descriptor.property->syNumberInLine); */
      /* Cast to intType: The line is probably in the range 0 to 2147483647 */
      result = (intType) obj_arg->descriptor.property->line;
    } else {
      result = 0;
    } /* if */
    return result;
  } /* refLine */



listType refLocalConsts (const const_objectType obj_arg)

  {
    listType local_elem;
    listType *list_insert_place;
    errInfoType err_info = OKAY_NO_ERROR;
    listType result;

  /* refLocalConsts */
    result = NULL;
    if (unlikely(obj_arg == NULL ||
                 CATEGORY_OF_OBJ(obj_arg) != BLOCKOBJECT)) {
      logError(printf("refLocalConsts(");
               trace1(obj_arg);
               printf("): Category is not BLOCKOBJECT.\n"););
      raise_error(RANGE_ERROR);
    } else {
      list_insert_place = &result;
      local_elem = obj_arg->value.blockValue->local_consts;
      while (local_elem != NULL) {
        list_insert_place = append_element_to_list(list_insert_place,
            local_elem->obj, &err_info);
        local_elem = local_elem->next;
      } /* while */
      if (unlikely(err_info != OKAY_NO_ERROR)) {
        free_list(result);
        result = NULL;
        raise_error(MEMORY_ERROR);
      } /* if */
    } /* if */
    return result;
  } /* refLocalConsts */



listType refLocalVars (const const_objectType obj_arg)

  {
    locListType local_elem;
    listType *list_insert_place;
    errInfoType err_info = OKAY_NO_ERROR;
    listType result;

  /* refLocalVars */
    result = NULL;
    if (unlikely(obj_arg == NULL ||
                 CATEGORY_OF_OBJ(obj_arg) != BLOCKOBJECT)) {
      logError(printf("refLocalVars(");
               trace1(obj_arg);
               printf("): Category is not BLOCKOBJECT.\n"););
      raise_error(RANGE_ERROR);
    } else {
      list_insert_place = &result;
      local_elem = obj_arg->value.blockValue->local_vars;
      while (local_elem != NULL) {
        list_insert_place = append_element_to_list(list_insert_place,
            local_elem->local.object, &err_info);
        local_elem = local_elem->next;
      } /* while */
      if (unlikely(err_info != OKAY_NO_ERROR)) {
        free_list(result);
        result = NULL;
        raise_error(MEMORY_ERROR);
      } /* if */
    } /* if */
    return result;
  } /* refLocalVars */



intType refNum (const const_objectType obj_arg)

  {
    static rtlHashType obj_table = NULL;
    static intType next_free_number = 1;
    intType result;

  /* refNum */
    logFunction(printf("refNum(" FMT_U_MEM ")\n", (memSizeType) obj_arg););
    if (unlikely(obj_arg == NULL)) {
      result = 0;
    } else {
      if (unlikely(obj_table == NULL)) {
        obj_table = hshEmpty();
      } /* if */
      if (unlikely(obj_table == NULL)) {
        raise_error(MEMORY_ERROR);
        result = 0;
      } else {
        result = (intType) hshIdxEnterDefault(obj_table, (genericType) (memSizeType) obj_arg,
            (genericType) next_free_number,
            (intType) (((memSizeType) obj_arg) >> 6), (compareType) &genericCmp,
            (createFuncType) &genericCreate, (createFuncType) &genericCreate);
        if (result == next_free_number) {
          next_free_number++;
        } /* if */
      } /* if */
    } /* if */
    logFunction(printf("refNum --> " FMT_D "\n", result););
    return result;
  } /* refNum */



listType refParams (const const_objectType obj_arg)

  {
    listType result;

  /* refParams */
    result = NULL;
    if (unlikely(obj_arg == NULL)) {
      logError(printf("refParams(NULL): Object is NULL.\n"););
      raise_error(RANGE_ERROR);
    } else {
      if (HAS_PROPERTY(obj_arg)) {
        result = obj_arg->descriptor.property->params;
      } /* if */
    } /* if */
    return result;
  } /* refParams */



objectType refResini (const const_objectType obj_arg)

  {
    objectType result;

  /* refResini */
    if (unlikely(obj_arg == NULL ||
                 CATEGORY_OF_OBJ(obj_arg) != BLOCKOBJECT)) {
      logError(printf("refResini(");
               trace1(obj_arg);
               printf("): Category is not BLOCKOBJECT.\n"););
      raise_error(RANGE_ERROR);
      result = NULL;
    } else {
      result = obj_arg->value.blockValue->result.init_value;
    } /* if */
    return result;
  } /* refResini */



objectType refResult (const const_objectType obj_arg)

  {
    objectType result;

  /* refResult */
    if (unlikely(obj_arg == NULL ||
                 CATEGORY_OF_OBJ(obj_arg) != BLOCKOBJECT)) {
      logError(printf("refResult(");
               trace1(obj_arg);
               printf("): Category is not BLOCKOBJECT.\n"););
      raise_error(RANGE_ERROR);
      result = NULL;
    } else {
      result = obj_arg->value.blockValue->result.object;
    } /* if */
    return result;
  } /* refResult */



listType refSctToList (const const_objectType obj_arg)

  {
    errInfoType err_info = OKAY_NO_ERROR;
    listType result;

  /* refSctToList */
    if (unlikely(obj_arg == NULL ||
                 CATEGORY_OF_OBJ(obj_arg) != STRUCTOBJECT)) {
      logError(printf("refSctToList(");
               trace1(obj_arg);
               printf("): Category is not STRUCTOBJECT.\n"););
      raise_error(RANGE_ERROR);
      result = NULL;
    } else {
      result = struct_to_list(take_struct(obj_arg), &err_info);
      if (unlikely(err_info != OKAY_NO_ERROR)) {
        raise_error(MEMORY_ERROR);
        result = NULL;
      } /* if */
    } /* if */
    return result;
  } /* refSctToList */



void refSetCategory (objectType obj_arg, intType aCategory)

  { /* refSetCategory */
    logFunction(printf("refSetCategory(");
                trace1(obj_arg);
                printf(", " FMT_D ")\n"););
    if (unlikely(obj_arg == NULL)) {
      logError(printf("refSetCategory(NULL, " FMT_D "): Object is NULL.\n",
                      aCategory););
      raise_error(RANGE_ERROR);
    } else {
      SET_CATEGORY_OF_OBJ(obj_arg, aCategory);
    } /* if */
  } /* refSetCategory */



void refSetParams (objectType obj_arg, const_listType params)

  {
    errInfoType err_info = OKAY_NO_ERROR;

  /* refSetParams */
    logFunction(printf("refSetParams(");
                trace1(obj_arg);
                printf(", ");
                prot_list(params);
                printf(")\n"););
    if (unlikely(obj_arg == NULL)) {
      logError(printf("refSetParams(NULL, " FMT_U_MEM "): Object is NULL.\n",
                      (memSizeType) params););
      raise_error(RANGE_ERROR);
    } else {
      if (HAS_PROPERTY(obj_arg)) {
        free_list(obj_arg->descriptor.property->params);
        obj_arg->descriptor.property->params = copy_list(params, &err_info);
      } /* if */
      if (CATEGORY_OF_OBJ(obj_arg) == BLOCKOBJECT) {
        obj_arg->value.blockValue->params =
            get_param_list(params, &err_info);
      } /* if */
      if (unlikely(err_info != OKAY_NO_ERROR)) {
        raise_error(MEMORY_ERROR);
      } /* if */
    } /* if */
  } /* refSetParams */



void refSetType (objectType obj_arg, typeType any_type)

  { /* refSetType */
    if (unlikely(obj_arg == NULL)) {
      logError(printf("refSetType(NULL, " FMT_U_MEM "): Object is NULL.\n",
                      (memSizeType) any_type););
      raise_error(RANGE_ERROR);
    } else {
      obj_arg->type_of = any_type;
    } /* if */
  } /* refSetType */



void refSetVar (objectType obj_arg, boolType var_flag)

  { /* refSetVar */
    if (unlikely(obj_arg == NULL)) {
      logError(printf("refSetVar(NULL, %s): Object is NULL.\n",
                      var_flag ? "TRUE" : "FALSE"););
      raise_error(RANGE_ERROR);
    } else if (var_flag) {
      SET_VAR_FLAG(obj_arg);
    } else {
      CLEAR_VAR_FLAG(obj_arg);
    } /* if */
  } /* refSetVar */



striType refStr (const const_objectType obj_arg)

  {
    const_cstriType stri;
    memSizeType buffer_len;
    char *buffer;
    listType name_elem;
    objectType param_obj;
    errInfoType err_info = OKAY_NO_ERROR;
    striType result;

  /* refStr */
    buffer = NULL;
    if (obj_arg == NULL) {
      stri = " *NULL_OBJECT* ";
    } else if (HAS_POSINFO(obj_arg)) {
      stri = (const_cstriType) get_file_name_ustri(GET_FILE_NUM(obj_arg));
      buffer_len = (memSizeType) strlen(stri) + 32;
      if (unlikely(!ALLOC_CSTRI(buffer, buffer_len))) {
        raise_error(MEMORY_ERROR);
        return NULL;
      } else {
        sprintf(buffer, "%s(%u)", stri, GET_LINE_NUM(obj_arg));
        stri = buffer;
      } /* if */
    } else if (!HAS_ENTITY(obj_arg)) {
      stri = " *NULL_ENTITY_OBJECT* ";
    } else if (GET_ENTITY(obj_arg)->ident != NULL) {
      stri = id_string2(GET_ENTITY(obj_arg)->ident);
    } else {
      stri = NULL;
      name_elem = GET_ENTITY(obj_arg)->fparam_list;
      while (name_elem != NULL && stri == NULL) {
        if (CATEGORY_OF_OBJ(name_elem->obj) == FORMPARAMOBJECT) {
          param_obj = name_elem->obj->value.objValue;
          if (CATEGORY_OF_OBJ(param_obj) != VALUEPARAMOBJECT &&
              CATEGORY_OF_OBJ(param_obj) != REFPARAMOBJECT &&
              CATEGORY_OF_OBJ(param_obj) != TYPEOBJECT) {
            stri = id_string2(GET_ENTITY(param_obj)->ident);
          } /* if */
        } else {
          stri = id_string2(GET_ENTITY(name_elem->obj)->ident);
        } /* if */
        name_elem = name_elem->next;
      } /* while */
      if (stri == NULL) {
        stri = " *UNKNOWN_NAME* ";
      } /* if */
    } /* if */
    result = cstri8_to_stri(stri, &err_info);
    if (buffer != NULL) {
      UNALLOC_CSTRI(buffer, buffer_len);
    } /* if */
    if (unlikely(result == NULL)) {
      raise_error(MEMORY_ERROR);
    } /* if */
    return result;
  } /* refStr */



typeType refType (const const_objectType obj_arg)

  {
    typeType result;

  /* refType */
    if (unlikely(obj_arg == NULL ||
                 obj_arg->type_of == NULL)) {
      logError(printf("refType(NULL): Object is NULL.\n"););
      raise_error(RANGE_ERROR);
      result = NULL;
    } else {
      result = obj_arg->type_of;
    } /* if */
    return result;
  } /* refType */



actType actValue (const const_objectType obj_arg)

  { /* actValue */
    if (unlikely(obj_arg == NULL ||
                 CATEGORY_OF_OBJ(obj_arg) != ACTOBJECT)) {
      logError(printf("actValue(");
               trace1(obj_arg);
               printf("): Category is not ACTOBJECT.\n"););
      raise_error(RANGE_ERROR);
      return NULL;
    } else {
      return take_action(obj_arg);
    } /* if */
  } /* actValue */



bigIntType bigValue (const const_objectType obj_arg)

  { /* bigValue */
    if (unlikely(obj_arg == NULL ||
                 CATEGORY_OF_OBJ(obj_arg) != BIGINTOBJECT)) {
      logError(printf("bigValue(");
               trace1(obj_arg);
               printf("): Category is not BIGINTOBJECT.\n"););
      raise_error(RANGE_ERROR);
      return NULL;
    } else {
      return bigCreate(take_bigint(obj_arg));
    } /* if */
  } /* bigValue */



boolType blnValue (const_objectType obj_arg)

  { /* blnValue */
    if (obj_arg != NULL) {
      if (CATEGORY_OF_OBJ(obj_arg) == CONSTENUMOBJECT ||
          CATEGORY_OF_OBJ(obj_arg) == VARENUMOBJECT) {
        obj_arg = obj_arg->value.objValue;
      } /* if */
      if (obj_arg->type_of != NULL &&
          obj_arg->type_of->owningProg != NULL) {
        if (obj_arg == TRUE_OBJECT(obj_arg->type_of->owningProg)) {
          return TRUE;
        } else if (obj_arg == FALSE_OBJECT(obj_arg->type_of->owningProg)) {
          return FALSE;
        } /* if */
      } /* if */
    } /* if */
    logError(printf("blnValue(");
             trace1(obj_arg);
             printf("): Value neither TRUE nor FALSE.\n"););
    raise_error(RANGE_ERROR);
    return FALSE;
  } /* blnValue */



bstriType bstValue (const const_objectType obj_arg)

  {
    bstriType bstri;
    bstriType result;

  /* bstValue */
    if (unlikely(obj_arg == NULL ||
                 CATEGORY_OF_OBJ(obj_arg) != BSTRIOBJECT ||
                 (bstri = take_bstri(obj_arg)) == NULL)) {
      logError(printf("bstValue(");
               trace1(obj_arg);
               printf("): Category is not BSTRIOBJECT.\n"););
      raise_error(RANGE_ERROR);
      result = NULL;
    } else {
      if (unlikely(!ALLOC_BSTRI_SIZE_OK(result, bstri->size))) {
        raise_error(MEMORY_ERROR);
      } else {
        result->size = bstri->size;
        memcpy(result->mem, bstri->mem,
            (size_t) (result->size * sizeof(ucharType)));
      } /* if */
    } /* if */
    return result;
  } /* bstValue */



charType chrValue (const const_objectType obj_arg)

  { /* chrValue */
    if (unlikely(obj_arg == NULL ||
                 CATEGORY_OF_OBJ(obj_arg) != CHAROBJECT)) {
      logError(printf("chrValue(");
               trace1(obj_arg);
               printf("): Category is not CHAROBJECT.\n"););
      raise_error(RANGE_ERROR);
      return '\0';
    } else {
      return take_char(obj_arg);
    } /* if */
  } /* chrValue */



winType drwValue (const const_objectType obj_arg)

  {
    winType win_value;

  /* drwValue */
    if (unlikely(obj_arg == NULL ||
                 CATEGORY_OF_OBJ(obj_arg) != WINOBJECT)) {
      logError(printf("drwValue(");
               trace1(obj_arg);
               printf("): Category is not WINOBJECT.\n"););
      raise_error(RANGE_ERROR);
      return NULL;
    } else {
      win_value = take_win(obj_arg);
      if (win_value != NULL) {
        win_value->usage_count++;
      } /* if */
      return win_value;
    } /* if */
  } /* drwValue */



fileType filValue (const const_objectType obj_arg)

  { /* filValue */
    if (unlikely(obj_arg == NULL ||
                 CATEGORY_OF_OBJ(obj_arg) != FILEOBJECT)) {
      logError(printf("filValue(");
               trace1(obj_arg);
               printf("): Category is not FILEOBJECT.\n"););
      raise_error(RANGE_ERROR);
      return NULL;
    } else {
      return take_file(obj_arg);
    } /* if */
  } /* filValue */



floatType fltValue (const const_objectType obj_arg)

  { /* fltValue */
    if (unlikely(obj_arg == NULL ||
                 CATEGORY_OF_OBJ(obj_arg) != FLOATOBJECT)) {
      logError(printf("fltValue(");
               trace1(obj_arg);
               printf("): Category is not FLOATOBJECT.\n"););
      raise_error(RANGE_ERROR);
      return 0.0;
    } else {
      return take_float(obj_arg);
    } /* if */
  } /* fltValue */



intType intValue (const const_objectType obj_arg)

  { /* intValue */
    if (unlikely(obj_arg == NULL ||
                 CATEGORY_OF_OBJ(obj_arg) != INTOBJECT)) {
      logError(printf("intValue(");
               trace1(obj_arg);
               printf("): Category is not INTOBJECT.\n"););
      raise_error(RANGE_ERROR);
      return 0;
    } else {
      return take_int(obj_arg);
    } /* if */
  } /* intValue */



processType pcsValue (const const_objectType obj_arg)

  {
    processType process_value;

  /* pcsValue */
    if (unlikely(obj_arg == NULL ||
                 CATEGORY_OF_OBJ(obj_arg) != PROCESSOBJECT)) {
      logError(printf("pcsValue(");
               trace1(obj_arg);
               printf("): Category is not PROCESSOBJECT.\n"););
      raise_error(RANGE_ERROR);
      return NULL;
    } else {
      process_value = take_process(obj_arg);
      if (process_value != NULL) {
        process_value->usage_count++;
      } /* if */
      return process_value;
    } /* if */
  } /* pcsValue */



pollType polValue (const const_objectType obj_arg)

  { /* polValue */
    if (unlikely(obj_arg == NULL ||
                 CATEGORY_OF_OBJ(obj_arg) != POLLOBJECT)) {
      logError(printf("polValue(");
               trace1(obj_arg);
               printf("): Category is not POLLOBJECT.\n"););
      raise_error(RANGE_ERROR);
      return NULL;
    } else {
      return polCreate(take_poll(obj_arg));
    } /* if */
  } /* polValue */



progType prgValue (const const_objectType obj_arg)

  { /* prgValue */
    if (unlikely(obj_arg == NULL ||
                 CATEGORY_OF_OBJ(obj_arg) != PROGOBJECT)) {
      logError(printf("prgValue(");
               trace1(obj_arg);
               printf("): Category is not PROGOBJECT.\n"););
      raise_error(RANGE_ERROR);
      return NULL;
    } else {
      return take_prog(obj_arg);
    } /* if */
  } /* prgValue */



objectType refValue (const const_objectType obj_arg)

  { /* refValue */
    if (likely(obj_arg != NULL &&
               (CATEGORY_OF_OBJ(obj_arg) == FWDREFOBJECT ||
                CATEGORY_OF_OBJ(obj_arg) == REFOBJECT ||
                CATEGORY_OF_OBJ(obj_arg) == REFPARAMOBJECT ||
                CATEGORY_OF_OBJ(obj_arg) == RESULTOBJECT ||
                CATEGORY_OF_OBJ(obj_arg) == LOCALVOBJECT ||
                CATEGORY_OF_OBJ(obj_arg) == ENUMLITERALOBJECT ||
                CATEGORY_OF_OBJ(obj_arg) == CONSTENUMOBJECT ||
                CATEGORY_OF_OBJ(obj_arg) == VARENUMOBJECT))) {
      return take_reference(obj_arg);
    } else {
      logError(printf("refValue(");
               trace1(obj_arg);
               printf("): Category is not a reference object.\n"););
      raise_error(RANGE_ERROR);
      return NULL;
    } /* if */
  } /* refValue */



listType rflValue (const const_objectType obj_arg)

  {
    errInfoType err_info = OKAY_NO_ERROR;
    listType result;

  /* rflValue */
    if (likely(obj_arg != NULL &&
               (CATEGORY_OF_OBJ(obj_arg) == MATCHOBJECT ||
                CATEGORY_OF_OBJ(obj_arg) == CALLOBJECT ||
                CATEGORY_OF_OBJ(obj_arg) == REFLISTOBJECT))) {
      result = copy_list(take_reflist(obj_arg), &err_info);
      if (unlikely(err_info != OKAY_NO_ERROR)) {
        raise_error(MEMORY_ERROR);
        result = NULL;
      } /* if */
    } else {
      logError(printf("rflValue(");
               trace1(obj_arg);
               printf("): Category is not a reference list object.\n"););
      raise_error(RANGE_ERROR);
      result = NULL;
    } /* if */
    return result;
  } /* rflValue */



void rflSetValue (objectType list_to, listType list_from)

  {
    listType help_list;
    errInfoType err_info = OKAY_NO_ERROR;

  /* rflSetValue */
    if (likely(CATEGORY_OF_OBJ(list_to) == MATCHOBJECT ||
               CATEGORY_OF_OBJ(list_to) == CALLOBJECT ||
               CATEGORY_OF_OBJ(list_to) == REFLISTOBJECT)) {
      help_list = copy_list(list_from, &err_info);
      if (unlikely(err_info != OKAY_NO_ERROR)) {
        raise_error(MEMORY_ERROR);
      } else {
        free_list(take_reflist(list_to));
        list_to->value.listValue = help_list;
      } /* if */
    } else {
      logError(printf("rflSetValue(");
               trace1(list_to);
               printf(", *): Category is not a reference list object.\n"););
      raise_error(RANGE_ERROR);
    } /* if */
  } /* rflSetValue */



setType setValue (const const_objectType obj_arg)

  {
    setType set1;
    memSizeType set_size;
    setType result;

  /* setValue */
    if (unlikely(obj_arg == NULL ||
                 CATEGORY_OF_OBJ(obj_arg) != SETOBJECT ||
                 (set1 = take_set(obj_arg)) == NULL)) {
      logError(printf("setValue(");
               trace1(obj_arg);
               printf("): Category is not SETOBJECT.\n"););
      raise_error(RANGE_ERROR);
      result = NULL;
    } else {
      set_size = bitsetSize(set1);
      if (unlikely(!ALLOC_SET(result, set_size))) {
        raise_error(MEMORY_ERROR);
      } else {
        result->min_position = set1->min_position;
        result->max_position = set1->max_position;
        memcpy(result->bitset, set1->bitset, set_size * sizeof(bitSetType));
      } /* if */
    } /* if */
    return result;
  } /* setValue */



striType strValue (const const_objectType obj_arg)

  {
    striType stri;
    striType result;

  /* strValue */
    if (unlikely(obj_arg == NULL ||
                 CATEGORY_OF_OBJ(obj_arg) != STRIOBJECT ||
                 (stri = take_stri(obj_arg)) == NULL)) {
      logError(printf("strValue(");
               trace1(obj_arg);
               printf("): Category is not STRIOBJECT.\n"););
      raise_error(RANGE_ERROR);
      result = NULL;
    } else {
      if (unlikely(!ALLOC_STRI_SIZE_OK(result, stri->size))) {
        raise_error(MEMORY_ERROR);
      } else {
        result->size = stri->size;
        memcpy(result->mem, stri->mem,
            (size_t) (result->size * sizeof(strElemType)));
      } /* if */
    } /* if */
    return result;
  } /* strValue */



typeType typValue (const const_objectType obj_arg)

  { /* typValue */
    if (unlikely(obj_arg == NULL ||
                 CATEGORY_OF_OBJ(obj_arg) != TYPEOBJECT)) {
      logError(printf("typValue(");
               trace1(obj_arg);
               printf("): Category is not TYPEOBJECT.\n"););
      raise_error(RANGE_ERROR);
      return NULL;
    } else {
      return take_type(obj_arg);
    } /* if */
  } /* typValue */
