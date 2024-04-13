ffi_closure_SYSV_inner (ffi_cif *cif,
			void (*fun)(ffi_cif*,void*,void**,void*),
			void *user_data,
			struct call_context *context,
			void *stack, void *rvalue, void *struct_rvalue)
{
  void **avalue = (void**) alloca (cif->nargs * sizeof (void*));
  int i, h, nargs, flags;
  struct arg_state state;

  arg_init (&state);

  for (i = 0, nargs = cif->nargs; i < nargs; i++)
    {
      ffi_type *ty = cif->arg_types[i];
      int t = ty->type;
      size_t n, s = ty->size;

      switch (t)
	{
	case FFI_TYPE_VOID:
	  FFI_ASSERT (0);
	  break;

	case FFI_TYPE_INT:
	case FFI_TYPE_UINT8:
	case FFI_TYPE_SINT8:
	case FFI_TYPE_UINT16:
	case FFI_TYPE_SINT16:
	case FFI_TYPE_UINT32:
	case FFI_TYPE_SINT32:
	case FFI_TYPE_UINT64:
	case FFI_TYPE_SINT64:
	case FFI_TYPE_POINTER:
	  avalue[i] = allocate_int_to_reg_or_stack (context, &state, stack, s);
	  break;

	case FFI_TYPE_FLOAT:
	case FFI_TYPE_DOUBLE:
	case FFI_TYPE_LONGDOUBLE:
	case FFI_TYPE_STRUCT:
	case FFI_TYPE_COMPLEX:
	  h = is_vfp_type (ty);
	  if (h)
	    {
	      n = 4 - (h & 3);
	      if (state.nsrn + n <= N_V_ARG_REG)
		{
		  void *reg = &context->v[state.nsrn];
		  state.nsrn += n;

		  /* Eeek! We need a pointer to the structure, however the
		     homogeneous float elements are being passed in individual
		     registers, therefore for float and double the structure
		     is not represented as a contiguous sequence of bytes in
		     our saved register context.  We don't need the original
		     contents of the register storage, so we reformat the
		     structure into the same memory.  */
		  avalue[i] = compress_hfa_type (reg, reg, h);
		}
	      else
		{
		  state.nsrn = N_V_ARG_REG;
		  avalue[i] = allocate_to_stack (&state, stack,
						 ty->alignment, s);
		}
	    }
	  else if (s > 16)
	    {
	      /* Replace Composite type of size greater than 16 with a
		 pointer.  */
	      avalue[i] = *(void **)
		allocate_int_to_reg_or_stack (context, &state, stack,
					      sizeof (void *));
	    }
	  else
	    {
	      n = (s + 7) / 8;
	      if (state.ngrn + n <= N_X_ARG_REG)
		{
		  avalue[i] = &context->x[state.ngrn];
		  state.ngrn += n;
		}
	      else
		{
		  state.ngrn = N_X_ARG_REG;
		  avalue[i] = allocate_to_stack (&state, stack,
						 ty->alignment, s);
		}
	    }
	  break;

	default:
	  abort();
	}

#if defined (__APPLE__)
      if (i + 1 == cif->aarch64_nfixedargs)
	{
	  state.ngrn = N_X_ARG_REG;
	  state.nsrn = N_V_ARG_REG;
	  state.allocating_variadic = 1;
	}
#endif
    }

  flags = cif->flags;
  if (flags & AARCH64_RET_IN_MEM)
    rvalue = struct_rvalue;

  fun (cif, rvalue, avalue, user_data);

  return flags;
}