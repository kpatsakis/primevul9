ffi_prep_cif_machdep (ffi_cif *cif)
{
  ffi_type *rtype = cif->rtype;
  size_t bytes = cif->bytes;
  int flags, i, n;

  switch (rtype->type)
    {
    case FFI_TYPE_VOID:
      flags = AARCH64_RET_VOID;
      break;
    case FFI_TYPE_UINT8:
      flags = AARCH64_RET_UINT8;
      break;
    case FFI_TYPE_UINT16:
      flags = AARCH64_RET_UINT16;
      break;
    case FFI_TYPE_UINT32:
      flags = AARCH64_RET_UINT32;
      break;
    case FFI_TYPE_SINT8:
      flags = AARCH64_RET_SINT8;
      break;
    case FFI_TYPE_SINT16:
      flags = AARCH64_RET_SINT16;
      break;
    case FFI_TYPE_INT:
    case FFI_TYPE_SINT32:
      flags = AARCH64_RET_SINT32;
      break;
    case FFI_TYPE_SINT64:
    case FFI_TYPE_UINT64:
      flags = AARCH64_RET_INT64;
      break;
    case FFI_TYPE_POINTER:
      flags = (sizeof(void *) == 4 ? AARCH64_RET_UINT32 : AARCH64_RET_INT64);
      break;

    case FFI_TYPE_FLOAT:
    case FFI_TYPE_DOUBLE:
    case FFI_TYPE_LONGDOUBLE:
    case FFI_TYPE_STRUCT:
    case FFI_TYPE_COMPLEX:
      flags = is_vfp_type (rtype);
      if (flags == 0)
	{
	  size_t s = rtype->size;
	  if (s > 16)
	    {
	      flags = AARCH64_RET_VOID | AARCH64_RET_IN_MEM;
	      bytes += 8;
	    }
	  else if (s == 16)
	    flags = AARCH64_RET_INT128;
	  else if (s == 8)
	    flags = AARCH64_RET_INT64;
	  else
	    flags = AARCH64_RET_INT128 | AARCH64_RET_NEED_COPY;
	}
      break;

    default:
      abort();
    }

  for (i = 0, n = cif->nargs; i < n; i++)
    if (is_vfp_type (cif->arg_types[i]))
      {
	flags |= AARCH64_FLAG_ARG_V;
	break;
      }

  /* Round the stack up to a multiple of the stack alignment requirement. */
  cif->bytes = (unsigned) FFI_ALIGN(bytes, 16);
  cif->flags = flags;
#if defined (__APPLE__)
  cif->aarch64_nfixedargs = 0;
#endif

  return FFI_OK;
}