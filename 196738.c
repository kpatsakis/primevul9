check_ppconst_bool(ppconst_T *ppconst)
{
    if (ppconst->pp_used > 0)
    {
	typval_T    *tv = &ppconst->pp_tv[ppconst->pp_used - 1];
	where_T	    where = WHERE_INIT;

	return check_typval_type(&t_bool, tv, where);
    }
    return OK;
}