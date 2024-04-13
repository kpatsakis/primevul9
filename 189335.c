insecure_flag(int opt_idx, int opt_flags)
{
    if (opt_flags & OPT_LOCAL)
	switch ((int)options[opt_idx].indir)
	{
#ifdef FEAT_STL_OPT
	    case PV_STL:	return &curwin->w_p_stl_flags;
#endif
#ifdef FEAT_EVAL
# ifdef FEAT_FOLDING
	    case PV_FDE:	return &curwin->w_p_fde_flags;
	    case PV_FDT:	return &curwin->w_p_fdt_flags;
# endif
# ifdef FEAT_BEVAL
	    case PV_BEXPR:	return &curbuf->b_p_bexpr_flags;
# endif
# if defined(FEAT_CINDENT)
	    case PV_INDE:	return &curbuf->b_p_inde_flags;
# endif
	    case PV_FEX:	return &curbuf->b_p_fex_flags;
# ifdef FEAT_FIND_ID
	    case PV_INEX:	return &curbuf->b_p_inex_flags;
# endif
#endif
	}

    /* Nothing special, return global flags field. */
    return &options[opt_idx].flags;
}