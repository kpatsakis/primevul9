did_set_option(
    int	    opt_idx,
    int	    opt_flags,	    /* possibly with OPT_MODELINE */
    int	    new_value)	    /* value was replaced completely */
{
    long_u	*p;

    options[opt_idx].flags |= P_WAS_SET;

    /* When an option is set in the sandbox, from a modeline or in secure mode
     * set the P_INSECURE flag.  Otherwise, if a new value is stored reset the
     * flag. */
    p = insecure_flag(opt_idx, opt_flags);
    if (secure
#ifdef HAVE_SANDBOX
	    || sandbox != 0
#endif
	    || (opt_flags & OPT_MODELINE))
	*p = *p | P_INSECURE;
    else if (new_value)
	*p = *p & ~P_INSECURE;
}