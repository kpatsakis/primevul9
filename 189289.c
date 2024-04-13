set_init_1(void)
{
    char_u	*p;
    int		opt_idx;
    long_u	n;

#ifdef FEAT_LANGMAP
    langmap_init();
#endif

    /* Be Vi compatible by default */
    p_cp = TRUE;

    /* Use POSIX compatibility when $VIM_POSIX is set. */
    if (mch_getenv((char_u *)"VIM_POSIX") != NULL)
    {
	set_string_default("cpo", (char_u *)CPO_ALL);
	set_string_default("shm", (char_u *)"A");
    }

    /*
     * Find default value for 'shell' option.
     * Don't use it if it is empty.
     */
    if (((p = mch_getenv((char_u *)"SHELL")) != NULL && *p != NUL)
#if defined(MSWIN)
	    || ((p = mch_getenv((char_u *)"COMSPEC")) != NULL && *p != NUL)
# ifdef WIN3264
	    || ((p = (char_u *)default_shell()) != NULL && *p != NUL)
# endif
#endif
	    )
	set_string_default("sh", p);

#ifdef FEAT_WILDIGN
    /*
     * Set the default for 'backupskip' to include environment variables for
     * temp files.
     */
    {
# ifdef UNIX
	static char	*(names[4]) = {"", "TMPDIR", "TEMP", "TMP"};
# else
	static char	*(names[3]) = {"TMPDIR", "TEMP", "TMP"};
# endif
	int		len;
	garray_T	ga;
	int		mustfree;

	ga_init2(&ga, 1, 100);
	for (n = 0; n < (long)(sizeof(names) / sizeof(char *)); ++n)
	{
	    mustfree = FALSE;
# ifdef UNIX
	    if (*names[n] == NUL)
		p = (char_u *)"/tmp";
	    else
# endif
		p = vim_getenv((char_u *)names[n], &mustfree);
	    if (p != NULL && *p != NUL)
	    {
		/* First time count the NUL, otherwise count the ','. */
		len = (int)STRLEN(p) + 3;
		if (ga_grow(&ga, len) == OK)
		{
		    if (ga.ga_len > 0)
			STRCAT(ga.ga_data, ",");
		    STRCAT(ga.ga_data, p);
		    add_pathsep(ga.ga_data);
		    STRCAT(ga.ga_data, "*");
		    ga.ga_len += len;
		}
	    }
	    if (mustfree)
		vim_free(p);
	}
	if (ga.ga_data != NULL)
	{
	    set_string_default("bsk", ga.ga_data);
	    vim_free(ga.ga_data);
	}
    }
#endif

    /*
     * 'maxmemtot' and 'maxmem' may have to be adjusted for available memory
     */
    opt_idx = findoption((char_u *)"maxmemtot");
    if (opt_idx >= 0)
    {
#if !defined(HAVE_AVAIL_MEM) && !defined(HAVE_TOTAL_MEM)
	if (options[opt_idx].def_val[VI_DEFAULT] == (char_u *)0L)
#endif
	{
#ifdef HAVE_AVAIL_MEM
	    /* Use amount of memory available at this moment. */
	    n = (mch_avail_mem(FALSE) >> 1);
#else
# ifdef HAVE_TOTAL_MEM
	    /* Use amount of memory available to Vim. */
	    n = (mch_total_mem(FALSE) >> 1);
# else
	    n = (0x7fffffff >> 11);
# endif
#endif
	    options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
	    opt_idx = findoption((char_u *)"maxmem");
	    if (opt_idx >= 0)
	    {
#if !defined(HAVE_AVAIL_MEM) && !defined(HAVE_TOTAL_MEM)
		if ((long)(long_i)options[opt_idx].def_val[VI_DEFAULT] > (long)n
		  || (long)(long_i)options[opt_idx].def_val[VI_DEFAULT] == 0L)
#endif
		    options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
	    }
	}
    }

#ifdef FEAT_SEARCHPATH
    {
	char_u	*cdpath;
	char_u	*buf;
	int	i;
	int	j;
	int	mustfree = FALSE;

	/* Initialize the 'cdpath' option's default value. */
	cdpath = vim_getenv((char_u *)"CDPATH", &mustfree);
	if (cdpath != NULL)
	{
	    buf = alloc((unsigned)((STRLEN(cdpath) << 1) + 2));
	    if (buf != NULL)
	    {
		buf[0] = ',';	    /* start with ",", current dir first */
		j = 1;
		for (i = 0; cdpath[i] != NUL; ++i)
		{
		    if (vim_ispathlistsep(cdpath[i]))
			buf[j++] = ',';
		    else
		    {
			if (cdpath[i] == ' ' || cdpath[i] == ',')
			    buf[j++] = '\\';
			buf[j++] = cdpath[i];
		    }
		}
		buf[j] = NUL;
		opt_idx = findoption((char_u *)"cdpath");
		if (opt_idx >= 0)
		{
		    options[opt_idx].def_val[VI_DEFAULT] = buf;
		    options[opt_idx].flags |= P_DEF_ALLOCED;
		}
		else
		    vim_free(buf); /* cannot happen */
	    }
	    if (mustfree)
		vim_free(cdpath);
	}
    }
#endif

#if defined(FEAT_POSTSCRIPT) && (defined(MSWIN) || defined(VMS) || defined(EBCDIC) || defined(MAC) || defined(hpux))
    /* Set print encoding on platforms that don't default to latin1 */
    set_string_default("penc",
# if defined(MSWIN)
		       (char_u *)"cp1252"
# else
#  ifdef VMS
		       (char_u *)"dec-mcs"
#  else
#   ifdef EBCDIC
		       (char_u *)"ebcdic-uk"
#   else
#    ifdef MAC
		       (char_u *)"mac-roman"
#    else /* HPUX */
		       (char_u *)"hp-roman8"
#    endif
#   endif
#  endif
# endif
		       );
#endif

#ifdef FEAT_POSTSCRIPT
    /* 'printexpr' must be allocated to be able to evaluate it. */
    set_string_default("pexpr",
# if defined(MSWIN)
	    (char_u *)"system('copy' . ' ' . v:fname_in . (&printdevice == '' ? ' LPT1:' : (' \"' . &printdevice . '\"'))) . delete(v:fname_in)"
# else
#  ifdef VMS
	    (char_u *)"system('print/delete' . (&printdevice == '' ? '' : ' /queue=' . &printdevice) . ' ' . v:fname_in)"

#  else
	    (char_u *)"system('lpr' . (&printdevice == '' ? '' : ' -P' . &printdevice) . ' ' . v:fname_in) . delete(v:fname_in) + v:shell_error"
#  endif
# endif
	    );
#endif

    /*
     * Set all the options (except the terminal options) to their default
     * value.  Also set the global value for local options.
     */
    set_options_default(0);

#ifdef FEAT_GUI
    if (found_reverse_arg)
	set_option_value((char_u *)"bg", 0L, (char_u *)"dark", 0);
#endif

    curbuf->b_p_initialized = TRUE;
    curbuf->b_p_ar = -1;	/* no local 'autoread' value */
    curbuf->b_p_ul = NO_LOCAL_UNDOLEVEL;
    check_buf_options(curbuf);
    check_win_options(curwin);
    check_options();

    /* Must be before option_expand(), because that one needs vim_isIDc() */
    didset_options();

#ifdef FEAT_SPELL
    /* Use the current chartab for the generic chartab. This is not in
     * didset_options() because it only depends on 'encoding'. */
    init_spell_chartab();
#endif

    /*
     * Expand environment variables and things like "~" for the defaults.
     * If option_expand() returns non-NULL the variable is expanded.  This can
     * only happen for non-indirect options.
     * Also set the default to the expanded value, so ":set" does not list
     * them.
     * Don't set the P_ALLOCED flag, because we don't want to free the
     * default.
     */
    for (opt_idx = 0; !istermoption(&options[opt_idx]); opt_idx++)
    {
	if ((options[opt_idx].flags & P_GETTEXT)
					      && options[opt_idx].var != NULL)
	    p = (char_u *)_(*(char **)options[opt_idx].var);
	else
	    p = option_expand(opt_idx, NULL);
	if (p != NULL && (p = vim_strsave(p)) != NULL)
	{
	    *(char_u **)options[opt_idx].var = p;
	    /* VIMEXP
	     * Defaults for all expanded options are currently the same for Vi
	     * and Vim.  When this changes, add some code here!  Also need to
	     * split P_DEF_ALLOCED in two.
	     */
	    if (options[opt_idx].flags & P_DEF_ALLOCED)
		vim_free(options[opt_idx].def_val[VI_DEFAULT]);
	    options[opt_idx].def_val[VI_DEFAULT] = p;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}
    }

    save_file_ff(curbuf);	/* Buffer is unchanged */

#if defined(FEAT_ARABIC)
    /* Detect use of mlterm.
     * Mlterm is a terminal emulator akin to xterm that has some special
     * abilities (bidi namely).
     * NOTE: mlterm's author is being asked to 'set' a variable
     *       instead of an environment variable due to inheritance.
     */
    if (mch_getenv((char_u *)"MLTERM") != NULL)
	set_option_value((char_u *)"tbidi", 1L, NULL, 0);
#endif

    didset_options2();

#ifdef FEAT_MBYTE
# if defined(WIN3264) && defined(FEAT_GETTEXT)
    /*
     * If $LANG isn't set, try to get a good value for it.  This makes the
     * right language be used automatically.  Don't do this for English.
     */
    if (mch_getenv((char_u *)"LANG") == NULL)
    {
	char	buf[20];

	/* Could use LOCALE_SISO639LANGNAME, but it's not in Win95.
	 * LOCALE_SABBREVLANGNAME gives us three letters, like "enu", we use
	 * only the first two. */
	n = GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SABBREVLANGNAME,
							     (LPTSTR)buf, 20);
	if (n >= 2 && STRNICMP(buf, "en", 2) != 0)
	{
	    /* There are a few exceptions (probably more) */
	    if (STRNICMP(buf, "cht", 3) == 0 || STRNICMP(buf, "zht", 3) == 0)
		STRCPY(buf, "zh_TW");
	    else if (STRNICMP(buf, "chs", 3) == 0
					      || STRNICMP(buf, "zhc", 3) == 0)
		STRCPY(buf, "zh_CN");
	    else if (STRNICMP(buf, "jp", 2) == 0)
		STRCPY(buf, "ja");
	    else
		buf[2] = NUL;		/* truncate to two-letter code */
	    vim_setenv((char_u *)"LANG", (char_u *)buf);
	}
    }
# else
#  ifdef MACOS_CONVERT
    /* Moved to os_mac_conv.c to avoid dependency problems. */
    mac_lang_init();
#  endif
# endif

    /* enc_locale() will try to find the encoding of the current locale. */
    p = enc_locale();
    if (p != NULL)
    {
	char_u *save_enc;

	/* Try setting 'encoding' and check if the value is valid.
	 * If not, go back to the default "latin1". */
	save_enc = p_enc;
	p_enc = p;
	if (STRCMP(p_enc, "gb18030") == 0)
	{
	    /* We don't support "gb18030", but "cp936" is a good substitute
	     * for practical purposes, thus use that.  It's not an alias to
	     * still support conversion between gb18030 and utf-8. */
	    p_enc = vim_strsave((char_u *)"cp936");
	    vim_free(p);
	}
	if (mb_init() == NULL)
	{
	    opt_idx = findoption((char_u *)"encoding");
	    if (opt_idx >= 0)
	    {
		options[opt_idx].def_val[VI_DEFAULT] = p_enc;
		options[opt_idx].flags |= P_DEF_ALLOCED;
	    }

#if defined(MSWIN) || defined(MACOS) || defined(VMS)
	    if (STRCMP(p_enc, "latin1") == 0
# ifdef FEAT_MBYTE
		    || enc_utf8
# endif
		    )
	    {
		/* Adjust the default for 'isprint' and 'iskeyword' to match
		 * latin1.  Also set the defaults for when 'nocompatible' is
		 * set. */
		set_string_option_direct((char_u *)"isp", -1,
					      ISP_LATIN1, OPT_FREE, SID_NONE);
		set_string_option_direct((char_u *)"isk", -1,
					      ISK_LATIN1, OPT_FREE, SID_NONE);
		opt_idx = findoption((char_u *)"isp");
		if (opt_idx >= 0)
		    options[opt_idx].def_val[VIM_DEFAULT] = ISP_LATIN1;
		opt_idx = findoption((char_u *)"isk");
		if (opt_idx >= 0)
		    options[opt_idx].def_val[VIM_DEFAULT] = ISK_LATIN1;
		(void)init_chartab();
	    }
#endif

# if defined(WIN3264) && !defined(FEAT_GUI)
	    /* Win32 console: When GetACP() returns a different value from
	     * GetConsoleCP() set 'termencoding'. */
	    if (GetACP() != GetConsoleCP())
	    {
		char	buf[50];

		sprintf(buf, "cp%ld", (long)GetConsoleCP());
		p_tenc = vim_strsave((char_u *)buf);
		if (p_tenc != NULL)
		{
		    opt_idx = findoption((char_u *)"termencoding");
		    if (opt_idx >= 0)
		    {
			options[opt_idx].def_val[VI_DEFAULT] = p_tenc;
			options[opt_idx].flags |= P_DEF_ALLOCED;
		    }
		    convert_setup(&input_conv, p_tenc, p_enc);
		    convert_setup(&output_conv, p_enc, p_tenc);
		}
		else
		    p_tenc = empty_option;
	    }
# endif
# if defined(WIN3264) && defined(FEAT_MBYTE)
	    /* $HOME may have characters in active code page. */
	    init_homedir();
# endif
	}
	else
	{
	    vim_free(p_enc);
	    p_enc = save_enc;
	}
    }
#endif

#ifdef FEAT_MULTI_LANG
    /* Set the default for 'helplang'. */
    set_helplang_default(get_mess_lang());
#endif
}