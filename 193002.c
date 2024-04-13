spell_suggest_file(suginfo_T *su, char_u *fname)
{
    FILE	*fd;
    char_u	line[MAXWLEN * 2];
    char_u	*p;
    int		len;
    char_u	cword[MAXWLEN];

    // Open the file.
    fd = mch_fopen((char *)fname, "r");
    if (fd == NULL)
    {
	semsg(_(e_notopen), fname);
	return;
    }

    // Read it line by line.
    while (!vim_fgets(line, MAXWLEN * 2, fd) && !got_int)
    {
	line_breakcheck();

	p = vim_strchr(line, '/');
	if (p == NULL)
	    continue;	    // No Tab found, just skip the line.
	*p++ = NUL;
	if (STRICMP(su->su_badword, line) == 0)
	{
	    // Match!  Isolate the good word, until CR or NL.
	    for (len = 0; p[len] >= ' '; ++len)
		;
	    p[len] = NUL;

	    // If the suggestion doesn't have specific case duplicate the case
	    // of the bad word.
	    if (captype(p, NULL) == 0)
	    {
		make_case_word(p, cword, su->su_badflags);
		p = cword;
	    }

	    add_suggestion(su, &su->su_ga, p, su->su_badlen,
				  SCORE_FILE, 0, TRUE, su->su_sallang, FALSE);
	}
    }

    fclose(fd);

    // Remove bogus suggestions, sort and truncate at "maxcount".
    check_suggestions(su, &su->su_ga);
    (void)cleanup_suggestions(&su->su_ga, su->su_maxscore, su->su_maxcount);
}