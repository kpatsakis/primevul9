fix_help_buffer(void)
{
    linenr_T	lnum;
    char_u	*line;
    int		in_example = FALSE;
    int		len;
    char_u	*fname;
    char_u	*p;
    char_u	*rt;
    int		mustfree;

    // Set filetype to "help" if still needed.
    if (STRCMP(curbuf->b_p_ft, "help") != 0)
    {
	++curbuf_lock;
	set_option_value((char_u *)"ft", 0L, (char_u *)"help", OPT_LOCAL);
	--curbuf_lock;
    }

#ifdef FEAT_SYN_HL
    if (!syntax_present(curwin))
#endif
    {
	for (lnum = 1; lnum <= curbuf->b_ml.ml_line_count; ++lnum)
	{
	    line = ml_get_buf(curbuf, lnum, FALSE);
	    len = (int)STRLEN(line);
	    if (in_example && len > 0 && !VIM_ISWHITE(line[0]))
	    {
		// End of example: non-white or '<' in first column.
		if (line[0] == '<')
		{
		    // blank-out a '<' in the first column
		    line = ml_get_buf(curbuf, lnum, TRUE);
		    line[0] = ' ';
		}
		in_example = FALSE;
	    }
	    if (!in_example && len > 0)
	    {
		if (line[len - 1] == '>' && (len == 1 || line[len - 2] == ' '))
		{
		    // blank-out a '>' in the last column (start of example)
		    line = ml_get_buf(curbuf, lnum, TRUE);
		    line[len - 1] = ' ';
		    in_example = TRUE;
		}
		else if (line[len - 1] == '~')
		{
		    // blank-out a '~' at the end of line (header marker)
		    line = ml_get_buf(curbuf, lnum, TRUE);
		    line[len - 1] = ' ';
		}
	    }
	}
    }

    // In the "help.txt" and "help.abx" file, add the locally added help
    // files.  This uses the very first line in the help file.
    fname = gettail(curbuf->b_fname);
    if (fnamecmp(fname, "help.txt") == 0
#ifdef FEAT_MULTI_LANG
	|| (fnamencmp(fname, "help.", 5) == 0
	    && ASCII_ISALPHA(fname[5])
	    && ASCII_ISALPHA(fname[6])
	    && TOLOWER_ASC(fname[7]) == 'x'
	    && fname[8] == NUL)
#endif
	)
    {
	for (lnum = 1; lnum < curbuf->b_ml.ml_line_count; ++lnum)
	{
	    line = ml_get_buf(curbuf, lnum, FALSE);
	    if (strstr((char *)line, "*local-additions*") == NULL)
		continue;

	    // Go through all directories in 'runtimepath', skipping
	    // $VIMRUNTIME.
	    p = p_rtp;
	    while (*p != NUL)
	    {
		copy_option_part(&p, NameBuff, MAXPATHL, ",");
		mustfree = FALSE;
		rt = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
		if (rt != NULL &&
			    fullpathcmp(rt, NameBuff, FALSE, TRUE) != FPC_SAME)
		{
		    int		fcount;
		    char_u	**fnames;
		    FILE	*fd;
		    char_u	*s;
		    int		fi;
		    vimconv_T	vc;
		    char_u	*cp;

		    // Find all "doc/ *.txt" files in this directory.
		    add_pathsep(NameBuff);
#ifdef FEAT_MULTI_LANG
		    STRCAT(NameBuff, "doc/*.??[tx]");
#else
		    STRCAT(NameBuff, "doc/*.txt");
#endif
		    if (gen_expand_wildcards(1, &NameBuff, &fcount,
					 &fnames, EW_FILE|EW_SILENT) == OK
			    && fcount > 0)
		    {
#ifdef FEAT_MULTI_LANG
			int	i1, i2;
			char_u	*f1, *f2;
			char_u	*t1, *t2;
			char_u	*e1, *e2;

			// If foo.abx is found use it instead of foo.txt in
			// the same directory.
			for (i1 = 0; i1 < fcount; ++i1)
			{
			    for (i2 = 0; i2 < fcount; ++i2)
			    {
				if (i1 == i2)
				    continue;
				if (fnames[i1] == NULL || fnames[i2] == NULL)
				    continue;
				f1 = fnames[i1];
				f2 = fnames[i2];
				t1 = gettail(f1);
				t2 = gettail(f2);
				e1 = vim_strrchr(t1, '.');
				e2 = vim_strrchr(t2, '.');
				if (e1 == NULL || e2 == NULL)
				    continue;
				if (fnamecmp(e1, ".txt") != 0
				    && fnamecmp(e1, fname + 4) != 0)
				{
				    // Not .txt and not .abx, remove it.
				    VIM_CLEAR(fnames[i1]);
				    continue;
				}
				if (e1 - f1 != e2 - f2
					    || fnamencmp(f1, f2, e1 - f1) != 0)
				    continue;
				if (fnamecmp(e1, ".txt") == 0
				    && fnamecmp(e2, fname + 4) == 0)
				    // use .abx instead of .txt
				    VIM_CLEAR(fnames[i1]);
			    }
			}
#endif
			for (fi = 0; fi < fcount; ++fi)
			{
			    if (fnames[fi] == NULL)
				continue;
			    fd = mch_fopen((char *)fnames[fi], "r");
			    if (fd != NULL)
			    {
				vim_fgets(IObuff, IOSIZE, fd);
				if (IObuff[0] == '*'
					&& (s = vim_strchr(IObuff + 1, '*'))
								  != NULL)
				{
				    int	this_utf = MAYBE;

				    // Change tag definition to a
				    // reference and remove <CR>/<NL>.
				    IObuff[0] = '|';
				    *s = '|';
				    while (*s != NUL)
				    {
					if (*s == '\r' || *s == '\n')
					    *s = NUL;
					// The text is utf-8 when a byte
					// above 127 is found and no
					// illegal byte sequence is found.
					if (*s >= 0x80 && this_utf != FALSE)
					{
					    int	l;

					    this_utf = TRUE;
					    l = utf_ptr2len(s);
					    if (l == 1)
						this_utf = FALSE;
					    s += l - 1;
					}
					++s;
				    }

				    // The help file is latin1 or utf-8;
				    // conversion to the current
				    // 'encoding' may be required.
				    vc.vc_type = CONV_NONE;
				    convert_setup(&vc, (char_u *)(
						this_utf == TRUE ? "utf-8"
						      : "latin1"), p_enc);
				    if (vc.vc_type == CONV_NONE)
					// No conversion needed.
					cp = IObuff;
				    else
				    {
					// Do the conversion.  If it fails
					// use the unconverted text.
					cp = string_convert(&vc, IObuff,
								    NULL);
					if (cp == NULL)
					    cp = IObuff;
				    }
				    convert_setup(&vc, NULL, NULL);

				    ml_append(lnum, cp, (colnr_T)0, FALSE);
				    if (cp != IObuff)
					vim_free(cp);
				    ++lnum;
				}
				fclose(fd);
			    }
			}
			FreeWild(fcount, fnames);
		    }
		}
		if (mustfree)
		    vim_free(rt);
	    }
	    break;
	}
    }
}