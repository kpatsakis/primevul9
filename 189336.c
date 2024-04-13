set_option_scriptID_idx(int opt_idx, int opt_flags, int id)
{
    int		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;
    int		indir = (int)options[opt_idx].indir;

    /* Remember where the option was set.  For local options need to do that
     * in the buffer or window structure. */
    if (both || (opt_flags & OPT_GLOBAL) || (indir & (PV_BUF|PV_WIN)) == 0)
	options[opt_idx].scriptID = id;
    if (both || (opt_flags & OPT_LOCAL))
    {
	if (indir & PV_BUF)
	    curbuf->b_p_scriptID[indir & PV_MASK] = id;
	else if (indir & PV_WIN)
	    curwin->w_p_scriptID[indir & PV_MASK] = id;
    }
}