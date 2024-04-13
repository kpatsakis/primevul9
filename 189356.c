makeset(FILE *fd, int opt_flags, int local_only)
{
    struct vimoption	*p;
    char_u		*varp;			/* currently used value */
    char_u		*varp_fresh;		/* local value */
    char_u		*varp_local = NULL;	/* fresh value */
    char		*cmd;
    int			round;
    int			pri;

    /*
     * The options that don't have a default (terminal name, columns, lines)
     * are never written.  Terminal options are also not written.
     * Do the loop over "options[]" twice: once for options with the
     * P_PRI_MKRC flag and once without.
     */
    for (pri = 1; pri >= 0; --pri)
    {
      for (p = &options[0]; !istermoption(p); p++)
	if (!(p->flags & P_NO_MKRC)
		&& !istermoption(p)
		&& ((pri == 1) == ((p->flags & P_PRI_MKRC) != 0)))
	{
	    /* skip global option when only doing locals */
	    if (p->indir == PV_NONE && !(opt_flags & OPT_GLOBAL))
		continue;

	    /* Do not store options like 'bufhidden' and 'syntax' in a vimrc
	     * file, they are always buffer-specific. */
	    if ((opt_flags & OPT_GLOBAL) && (p->flags & P_NOGLOB))
		continue;

	    /* Global values are only written when not at the default value. */
	    varp = get_varp_scope(p, opt_flags);
	    if ((opt_flags & OPT_GLOBAL) && optval_default(p, varp))
		continue;

	    round = 2;
	    if (p->indir != PV_NONE)
	    {
		if (p->var == VAR_WIN)
		{
		    /* skip window-local option when only doing globals */
		    if (!(opt_flags & OPT_LOCAL))
			continue;
		    /* When fresh value of window-local option is not at the
		     * default, need to write it too. */
		    if (!(opt_flags & OPT_GLOBAL) && !local_only)
		    {
			varp_fresh = get_varp_scope(p, OPT_GLOBAL);
			if (!optval_default(p, varp_fresh))
			{
			    round = 1;
			    varp_local = varp;
			    varp = varp_fresh;
			}
		    }
		}
	    }

	    /* Round 1: fresh value for window-local options.
	     * Round 2: other values */
	    for ( ; round <= 2; varp = varp_local, ++round)
	    {
		if (round == 1 || (opt_flags & OPT_GLOBAL))
		    cmd = "set";
		else
		    cmd = "setlocal";

		if (p->flags & P_BOOL)
		{
		    if (put_setbool(fd, cmd, p->fullname, *(int *)varp) == FAIL)
			return FAIL;
		}
		else if (p->flags & P_NUM)
		{
		    if (put_setnum(fd, cmd, p->fullname, (long *)varp) == FAIL)
			return FAIL;
		}
		else    /* P_STRING */
		{
#if defined(FEAT_SYN_HL) || defined(FEAT_AUTOCMD)
		    int		do_endif = FALSE;

		    /* Don't set 'syntax' and 'filetype' again if the value is
		     * already right, avoids reloading the syntax file. */
		    if (
# if defined(FEAT_SYN_HL)
			    p->indir == PV_SYN
#  if defined(FEAT_AUTOCMD)
			    ||
#  endif
# endif
# if defined(FEAT_AUTOCMD)
			    p->indir == PV_FT
# endif
			    )
		    {
			if (fprintf(fd, "if &%s != '%s'", p->fullname,
						       *(char_u **)(varp)) < 0
				|| put_eol(fd) < 0)
			    return FAIL;
			do_endif = TRUE;
		    }
#endif
		    if (put_setstring(fd, cmd, p->fullname, (char_u **)varp,
					  (p->flags & P_EXPAND) != 0) == FAIL)
			return FAIL;
#if defined(FEAT_SYN_HL) || defined(FEAT_AUTOCMD)
		    if (do_endif)
		    {
			if (put_line(fd, "endif") == FAIL)
			    return FAIL;
		    }
#endif
		}
	    }
	}
    }
    return OK;
}