option_iter_next(void **option, int opt_type)
{
    struct vimoption	*ret = NULL;
    do
    {
	if (*option == NULL)
	    *option = (void *) options;
	else if (((struct vimoption *) (*option))->fullname == NULL)
	{
	    *option = NULL;
	    return NULL;
	}
	else
	    *option = (void *) (((struct vimoption *) (*option)) + 1);

	ret = ((struct vimoption *) (*option));

	/* Hidden option */
	if (ret->var == NULL)
	{
	    ret = NULL;
	    continue;
	}

	switch (opt_type)
	{
	    case SREQ_GLOBAL:
		if (!(ret->indir == PV_NONE || ret->indir & PV_BOTH))
		    ret = NULL;
		break;
	    case SREQ_BUF:
		if (!(ret->indir & PV_BUF))
		    ret = NULL;
		break;
	    case SREQ_WIN:
		if (!(ret->indir & PV_WIN))
		    ret = NULL;
		break;
	    default:
		EMSG2(_(e_intern2), "option_iter_next()");
		return NULL;
	}
    }
    while (ret == NULL);

    return (char_u *)ret->fullname;
}