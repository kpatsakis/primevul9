set_term_option_alloced(char_u **p)
{
    int		opt_idx;

    for (opt_idx = 1; options[opt_idx].fullname != NULL; opt_idx++)
	if (options[opt_idx].var == (char_u *)p)
	{
	    options[opt_idx].flags |= P_ALLOCED;
	    return;
	}
    return; /* cannot happen: didn't find it! */
}