bool_on_stack(cctx_T *cctx)
{
    type_T	*type;

    type = get_type_on_stack(cctx, 0);
    if (type == &t_bool)
	return OK;

    if (type == &t_any
	    || type == &t_unknown
	    || type == &t_number
	    || type == &t_number_bool)
	// Number 0 and 1 are OK to use as a bool.  "any" could also be a bool.
	// This requires a runtime type check.
	return generate_COND2BOOL(cctx);

    return need_type(type, &t_bool, -1, 0, cctx, FALSE, FALSE);
}