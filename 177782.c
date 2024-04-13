extend_integer_type (void *source, int type)
{
  switch (type)
    {
    case FFI_TYPE_UINT8:
      return *(UINT8 *) source;
    case FFI_TYPE_SINT8:
      return *(SINT8 *) source;
    case FFI_TYPE_UINT16:
      return *(UINT16 *) source;
    case FFI_TYPE_SINT16:
      return *(SINT16 *) source;
    case FFI_TYPE_UINT32:
      return *(UINT32 *) source;
    case FFI_TYPE_INT:
    case FFI_TYPE_SINT32:
      return *(SINT32 *) source;
    case FFI_TYPE_UINT64:
    case FFI_TYPE_SINT64:
      return *(UINT64 *) source;
      break;
    case FFI_TYPE_POINTER:
      return *(uintptr_t *) source;
    default:
      abort();
    }
}