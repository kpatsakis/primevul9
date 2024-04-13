is_hfa0 (const ffi_type *ty)
{
  ffi_type **elements = ty->elements;
  int i, ret = -1;

  if (elements != NULL)
    for (i = 0; elements[i]; ++i)
      {
        ret = elements[i]->type;
        if (ret == FFI_TYPE_STRUCT || ret == FFI_TYPE_COMPLEX)
          {
            ret = is_hfa0 (elements[i]);
            if (ret < 0)
              continue;
          }
        break;
      }

  return ret;
}