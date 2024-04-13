ffi_call_int (ffi_cif *cif, void (*fn)(void), void *orig_rvalue,
	      void **avalue, void *closure)
{
  struct call_context *context;
  void *stack, *frame, *rvalue;
  struct arg_state state;
  size_t stack_bytes, rtype_size, rsize;
  int i, nargs, flags;
  ffi_type *rtype;

  flags = cif->flags;
  rtype = cif->rtype;
  rtype_size = rtype->size;
  stack_bytes = cif->bytes;

  /* If the target function returns a structure via hidden pointer,
     then we cannot allow a null rvalue.  Otherwise, mash a null
     rvalue to void return type.  */
  rsize = 0;
  if (flags & AARCH64_RET_IN_MEM)
    {
      if (orig_rvalue == NULL)
	rsize = rtype_size;
    }
  else if (orig_rvalue == NULL)
    flags &= AARCH64_FLAG_ARG_V;
  else if (flags & AARCH64_RET_NEED_COPY)
    rsize = 16;

  /* Allocate consectutive stack for everything we'll need.  */
  context = alloca (sizeof(struct call_context) + stack_bytes + 32 + rsize);
  stack = context + 1;
  frame = stack + stack_bytes;
  rvalue = (rsize ? frame + 32 : orig_rvalue);

  arg_init (&state);
  for (i = 0, nargs = cif->nargs; i < nargs; i++)
    {
      ffi_type *ty = cif->arg_types[i];
      size_t s = ty->size;
      void *a = avalue[i];
      int h, t;

      t = ty->type;
      switch (t)
	{
	case FFI_TYPE_VOID:
	  FFI_ASSERT (0);
	  break;

	/* If the argument is a basic type the argument is allocated to an
	   appropriate register, or if none are available, to the stack.  */
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
	do_pointer:
	  {
	    ffi_arg ext = extend_integer_type (a, t);
	    if (state.ngrn < N_X_ARG_REG)
	      context->x[state.ngrn++] = ext;
	    else
	      {
		void *d = allocate_to_stack (&state, stack, ty->alignment, s);
		state.ngrn = N_X_ARG_REG;
		/* Note that the default abi extends each argument
		   to a full 64-bit slot, while the iOS abi allocates
		   only enough space. */
#ifdef __APPLE__
		memcpy(d, a, s);
#else
		*(ffi_arg *)d = ext;
#endif
	      }
	  }
	  break;

	case FFI_TYPE_FLOAT:
	case FFI_TYPE_DOUBLE:
	case FFI_TYPE_LONGDOUBLE:
	case FFI_TYPE_STRUCT:
	case FFI_TYPE_COMPLEX:
	  {
	    void *dest;

	    h = is_vfp_type (ty);
	    if (h)
	      {
		int elems = 4 - (h & 3);
	        if (state.nsrn + elems <= N_V_ARG_REG)
		  {
		    dest = &context->v[state.nsrn];
		    state.nsrn += elems;
		    extend_hfa_type (dest, a, h);
		    break;
		  }
		state.nsrn = N_V_ARG_REG;
		dest = allocate_to_stack (&state, stack, ty->alignment, s);
	      }
	    else if (s > 16)
	      {
		/* If the argument is a composite type that is larger than 16
		   bytes, then the argument has been copied to memory, and
		   the argument is replaced by a pointer to the copy.  */
		a = &avalue[i];
		t = FFI_TYPE_POINTER;
		s = sizeof (void *);
		goto do_pointer;
	      }
	    else
	      {
		size_t n = (s + 7) / 8;
		if (state.ngrn + n <= N_X_ARG_REG)
		  {
		    /* If the argument is a composite type and the size in
		       double-words is not more than the number of available
		       X registers, then the argument is copied into
		       consecutive X registers.  */
		    dest = &context->x[state.ngrn];
		    state.ngrn += n;
		  }
		else
		  {
		    /* Otherwise, there are insufficient X registers. Further
		       X register allocations are prevented, the NSAA is
		       adjusted and the argument is copied to memory at the
		       adjusted NSAA.  */
		    state.ngrn = N_X_ARG_REG;
		    dest = allocate_to_stack (&state, stack, ty->alignment, s);
		  }
		}
	      memcpy (dest, a, s);
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

  ffi_call_SYSV (context, frame, fn, rvalue, flags, closure);

  if (flags & AARCH64_RET_NEED_COPY)
    memcpy (orig_rvalue, rvalue, rtype_size);
}