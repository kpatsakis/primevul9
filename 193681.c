helptags_one(
    char_u	*dir,		// doc directory
    char_u	*ext,		// suffix, ".txt", ".itx", ".frx", etc.
    char_u	*tagfname,	// "tags" for English, "tags-fr" for French.
    int		add_help_tags,	// add "help-tags" tag
    int		ignore_writeerr)    // ignore write error
{
    FILE	*fd_tags;
    FILE	*fd;
    garray_T	ga;
    int		filecount;
    char_u	**files;
    char_u	*p1, *p2;
    int		fi;
    char_u	*s;
    int		i;
    char_u	*fname;
    int		dirlen;
    int		utf8 = MAYBE;
    int		this_utf8;
    int		firstline;
    int		mix = FALSE;	// detected mixed encodings

    // Find all *.txt files.
    dirlen = (int)STRLEN(dir);
    STRCPY(NameBuff, dir);
    STRCAT(NameBuff, "/**/*");
    STRCAT(NameBuff, ext);
    if (gen_expand_wildcards(1, &NameBuff, &filecount, &files,
						    EW_FILE|EW_SILENT) == FAIL
	    || filecount == 0)
    {
	if (!got_int)
	    semsg(_("E151: No match: %s"), NameBuff);
	return;
    }

    // Open the tags file for writing.
    // Do this before scanning through all the files.
    STRCPY(NameBuff, dir);
    add_pathsep(NameBuff);
    STRCAT(NameBuff, tagfname);
    fd_tags = mch_fopen((char *)NameBuff, "w");
    if (fd_tags == NULL)
    {
	if (!ignore_writeerr)
	    semsg(_("E152: Cannot open %s for writing"), NameBuff);
	FreeWild(filecount, files);
	return;
    }

    // If using the "++t" argument or generating tags for "$VIMRUNTIME/doc"
    // add the "help-tags" tag.
    ga_init2(&ga, (int)sizeof(char_u *), 100);
    if (add_help_tags || fullpathcmp((char_u *)"$VIMRUNTIME/doc",
						dir, FALSE, TRUE) == FPC_SAME)
    {
	if (ga_grow(&ga, 1) == FAIL)
	    got_int = TRUE;
	else
	{
	    s = alloc(18 + (unsigned)STRLEN(tagfname));
	    if (s == NULL)
		got_int = TRUE;
	    else
	    {
		sprintf((char *)s, "help-tags\t%s\t1\n", tagfname);
		((char_u **)ga.ga_data)[ga.ga_len] = s;
		++ga.ga_len;
	    }
	}
    }

    // Go over all the files and extract the tags.
    for (fi = 0; fi < filecount && !got_int; ++fi)
    {
	fd = mch_fopen((char *)files[fi], "r");
	if (fd == NULL)
	{
	    semsg(_("E153: Unable to open %s for reading"), files[fi]);
	    continue;
	}
	fname = files[fi] + dirlen + 1;

	firstline = TRUE;
	while (!vim_fgets(IObuff, IOSIZE, fd) && !got_int)
	{
	    if (firstline)
	    {
		// Detect utf-8 file by a non-ASCII char in the first line.
		this_utf8 = MAYBE;
		for (s = IObuff; *s != NUL; ++s)
		    if (*s >= 0x80)
		    {
			int l;

			this_utf8 = TRUE;
			l = utf_ptr2len(s);
			if (l == 1)
			{
			    // Illegal UTF-8 byte sequence.
			    this_utf8 = FALSE;
			    break;
			}
			s += l - 1;
		    }
		if (this_utf8 == MAYBE)	    // only ASCII characters found
		    this_utf8 = FALSE;
		if (utf8 == MAYBE)	    // first file
		    utf8 = this_utf8;
		else if (utf8 != this_utf8)
		{
		    semsg(_("E670: Mix of help file encodings within a language: %s"), files[fi]);
		    mix = !got_int;
		    got_int = TRUE;
		}
		firstline = FALSE;
	    }
	    p1 = vim_strchr(IObuff, '*');	// find first '*'
	    while (p1 != NULL)
	    {
		// Use vim_strbyte() instead of vim_strchr() so that when
		// 'encoding' is dbcs it still works, don't find '*' in the
		// second byte.
		p2 = vim_strbyte(p1 + 1, '*');	// find second '*'
		if (p2 != NULL && p2 > p1 + 1)	// skip "*" and "**"
		{
		    for (s = p1 + 1; s < p2; ++s)
			if (*s == ' ' || *s == '\t' || *s == '|')
			    break;

		    // Only accept a *tag* when it consists of valid
		    // characters, there is white space before it and is
		    // followed by a white character or end-of-line.
		    if (s == p2
			    && (p1 == IObuff || p1[-1] == ' ' || p1[-1] == '\t')
			    && (vim_strchr((char_u *)" \t\n\r", s[1]) != NULL
				|| s[1] == '\0'))
		    {
			*p2 = '\0';
			++p1;
			if (ga_grow(&ga, 1) == FAIL)
			{
			    got_int = TRUE;
			    break;
			}
			s = alloc(p2 - p1 + STRLEN(fname) + 2);
			if (s == NULL)
			{
			    got_int = TRUE;
			    break;
			}
			((char_u **)ga.ga_data)[ga.ga_len] = s;
			++ga.ga_len;
			sprintf((char *)s, "%s\t%s", p1, fname);

			// find next '*'
			p2 = vim_strchr(p2 + 1, '*');
		    }
		}
		p1 = p2;
	    }
	    line_breakcheck();
	}

	fclose(fd);
    }

    FreeWild(filecount, files);

    if (!got_int)
    {
	// Sort the tags.
	if (ga.ga_data != NULL)
	    sort_strings((char_u **)ga.ga_data, ga.ga_len);

	// Check for duplicates.
	for (i = 1; i < ga.ga_len; ++i)
	{
	    p1 = ((char_u **)ga.ga_data)[i - 1];
	    p2 = ((char_u **)ga.ga_data)[i];
	    while (*p1 == *p2)
	    {
		if (*p2 == '\t')
		{
		    *p2 = NUL;
		    vim_snprintf((char *)NameBuff, MAXPATHL,
			    _("E154: Duplicate tag \"%s\" in file %s/%s"),
				     ((char_u **)ga.ga_data)[i], dir, p2 + 1);
		    emsg((char *)NameBuff);
		    *p2 = '\t';
		    break;
		}
		++p1;
		++p2;
	    }
	}

	if (utf8 == TRUE)
	    fprintf(fd_tags, "!_TAG_FILE_ENCODING\tutf-8\t//\n");

	// Write the tags into the file.
	for (i = 0; i < ga.ga_len; ++i)
	{
	    s = ((char_u **)ga.ga_data)[i];
	    if (STRNCMP(s, "help-tags\t", 10) == 0)
		// help-tags entry was added in formatted form
		fputs((char *)s, fd_tags);
	    else
	    {
		fprintf(fd_tags, "%s\t/*", s);
		for (p1 = s; *p1 != '\t'; ++p1)
		{
		    // insert backslash before '\\' and '/'
		    if (*p1 == '\\' || *p1 == '/')
			putc('\\', fd_tags);
		    putc(*p1, fd_tags);
		}
		fprintf(fd_tags, "*\n");
	    }
	}
    }
    if (mix)
	got_int = FALSE;    // continue with other languages

    for (i = 0; i < ga.ga_len; ++i)
	vim_free(((char_u **)ga.ga_data)[i]);
    ga_clear(&ga);
    fclose(fd_tags);	    // there is no check for an error...
}