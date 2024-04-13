get_winbuf_options(int bufopt)
{
    dict_T	*d;
    int		opt_idx;

    d = dict_alloc();
    if (d == NULL)
	return NULL;

    for (opt_idx = 0; !istermoption(&options[opt_idx]); opt_idx++)
    {
	struct vimoption *opt = &options[opt_idx];

	if ((bufopt && (opt->indir & PV_BUF))
					 || (!bufopt && (opt->indir & PV_WIN)))
	{
	    char_u *varp = get_varp(opt);

	    if (varp != NULL)
	    {
		if (opt->flags & P_STRING)
		    dict_add_nr_str(d, opt->fullname, 0L, *(char_u **)varp);
		else if (opt->flags & P_NUM)
		    dict_add_nr_str(d, opt->fullname, *(long *)varp, NULL);
		else
		    dict_add_nr_str(d, opt->fullname, *(int *)varp, NULL);
	    }
	}
    }

    return d;
}