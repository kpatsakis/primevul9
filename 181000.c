handle_closure_in_use(ectx_T *ectx, int free_arguments)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;
    int		argcount;
    int		top;
    int		idx;
    typval_T	*tv;
    int		closure_in_use = FALSE;
    garray_T	*gap = &ectx->ec_funcrefs;
    varnumber_T	closure_count;

    if (dfunc->df_ufunc == NULL)
	return OK;  // function was freed
    if (dfunc->df_has_closure == 0)
	return OK;  // no closures
    tv = STACK_TV(ectx->ec_frame_idx + STACK_FRAME_SIZE + dfunc->df_varcount);
    closure_count = tv->vval.v_number;
    if (closure_count == 0)
	return OK;  // no funcrefs created

    argcount = ufunc_argcount(dfunc->df_ufunc);
    top = ectx->ec_frame_idx - argcount;

    // Check if any created closure is still in use.
    for (idx = 0; idx < closure_count; ++idx)
    {
	partial_T   *pt;
	int	    off = gap->ga_len - closure_count + idx;

	if (off < 0)
	    continue;  // count is off or already done
	pt = ((partial_T **)gap->ga_data)[off];
	if (pt->pt_refcount > 1)
	{
	    int refcount = pt->pt_refcount;
	    int i;

	    // A Reference in a local variable doesn't count, it gets
	    // unreferenced on return.
	    for (i = 0; i < dfunc->df_varcount; ++i)
	    {
		typval_T *stv = STACK_TV(ectx->ec_frame_idx
						       + STACK_FRAME_SIZE + i);
		if (stv->v_type == VAR_PARTIAL && pt == stv->vval.v_partial)
		    --refcount;
	    }
	    if (refcount > 1)
	    {
		closure_in_use = TRUE;
		break;
	    }
	}
    }

    if (closure_in_use)
    {
	funcstack_T *funcstack = ALLOC_CLEAR_ONE(funcstack_T);
	typval_T    *stack;

	// A closure is using the arguments and/or local variables.
	// Move them to the called function.
	if (funcstack == NULL)
	    return FAIL;

	funcstack->fs_var_offset = argcount + STACK_FRAME_SIZE;
	funcstack->fs_ga.ga_len = funcstack->fs_var_offset + dfunc->df_varcount;
	stack = ALLOC_CLEAR_MULT(typval_T, funcstack->fs_ga.ga_len);
	funcstack->fs_ga.ga_data = stack;
	if (stack == NULL)
	{
	    vim_free(funcstack);
	    return FAIL;
	}
	add_funcstack_to_list(funcstack);

	// Move or copy the arguments.
	for (idx = 0; idx < argcount; ++idx)
	{
	    tv = STACK_TV(top + idx);
	    if (free_arguments)
	    {
		*(stack + idx) = *tv;
		tv->v_type = VAR_UNKNOWN;
	    }
	    else
		copy_tv(tv, stack + idx);
	}
	// Move the local variables.
	for (idx = 0; idx < dfunc->df_varcount; ++idx)
	{
	    tv = STACK_TV(ectx->ec_frame_idx + STACK_FRAME_SIZE + idx);

	    // A partial created for a local function, that is also used as a
	    // local variable, has a reference count for the variable, thus
	    // will never go down to zero.  When all these refcounts are one
	    // then the funcstack is unused.  We need to count how many we have
	    // so we know when to check.
	    if (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != NULL)
	    {
		int	    i;

		for (i = 0; i < closure_count; ++i)
		    if (tv->vval.v_partial == ((partial_T **)gap->ga_data)[
					      gap->ga_len - closure_count + i])
			++funcstack->fs_min_refcount;
	    }

	    *(stack + funcstack->fs_var_offset + idx) = *tv;
	    tv->v_type = VAR_UNKNOWN;
	}

	for (idx = 0; idx < closure_count; ++idx)
	{
	    partial_T *pt = ((partial_T **)gap->ga_data)[gap->ga_len
							- closure_count + idx];
	    if (pt->pt_refcount > 1)
	    {
		++funcstack->fs_refcount;
		pt->pt_funcstack = funcstack;
		pt->pt_outer.out_stack = &funcstack->fs_ga;
		pt->pt_outer.out_frame_idx = ectx->ec_frame_idx - top;
	    }
	}
    }

    for (idx = 0; idx < closure_count; ++idx)
	partial_unref(((partial_T **)gap->ga_data)[gap->ga_len
						       - closure_count + idx]);
    gap->ga_len -= closure_count;
    if (gap->ga_len == 0)
	ga_clear(gap);

    return OK;
}