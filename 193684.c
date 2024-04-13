do_helptags(char_u *dirname, int add_help_tags, int ignore_writeerr)
{
#ifdef FEAT_MULTI_LANG
    int		len;
    int		i, j;
    garray_T	ga;
    char_u	lang[2];
    char_u	ext[5];
    char_u	fname[8];
    int		filecount;
    char_u	**files;

    // Get a list of all files in the help directory and in subdirectories.
    STRCPY(NameBuff, dirname);
    add_pathsep(NameBuff);
    STRCAT(NameBuff, "**");
    if (gen_expand_wildcards(1, &NameBuff, &filecount, &files,
						    EW_FILE|EW_SILENT) == FAIL
	    || filecount == 0)
    {
	semsg(_("E151: No match: %s"), NameBuff);
	return;
    }

    // Go over all files in the directory to find out what languages are
    // present.
    ga_init2(&ga, 1, 10);
    for (i = 0; i < filecount; ++i)
    {
	len = (int)STRLEN(files[i]);
	if (len > 4)
	{
	    if (STRICMP(files[i] + len - 4, ".txt") == 0)
	    {
		// ".txt" -> language "en"
		lang[0] = 'e';
		lang[1] = 'n';
	    }
	    else if (files[i][len - 4] == '.'
		    && ASCII_ISALPHA(files[i][len - 3])
		    && ASCII_ISALPHA(files[i][len - 2])
		    && TOLOWER_ASC(files[i][len - 1]) == 'x')
	    {
		// ".abx" -> language "ab"
		lang[0] = TOLOWER_ASC(files[i][len - 3]);
		lang[1] = TOLOWER_ASC(files[i][len - 2]);
	    }
	    else
		continue;

	    // Did we find this language already?
	    for (j = 0; j < ga.ga_len; j += 2)
		if (STRNCMP(lang, ((char_u *)ga.ga_data) + j, 2) == 0)
		    break;
	    if (j == ga.ga_len)
	    {
		// New language, add it.
		if (ga_grow(&ga, 2) == FAIL)
		    break;
		((char_u *)ga.ga_data)[ga.ga_len++] = lang[0];
		((char_u *)ga.ga_data)[ga.ga_len++] = lang[1];
	    }
	}
    }

    // Loop over the found languages to generate a tags file for each one.
    for (j = 0; j < ga.ga_len; j += 2)
    {
	STRCPY(fname, "tags-xx");
	fname[5] = ((char_u *)ga.ga_data)[j];
	fname[6] = ((char_u *)ga.ga_data)[j + 1];
	if (fname[5] == 'e' && fname[6] == 'n')
	{
	    // English is an exception: use ".txt" and "tags".
	    fname[4] = NUL;
	    STRCPY(ext, ".txt");
	}
	else
	{
	    // Language "ab" uses ".abx" and "tags-ab".
	    STRCPY(ext, ".xxx");
	    ext[1] = fname[5];
	    ext[2] = fname[6];
	}
	helptags_one(dirname, ext, fname, add_help_tags, ignore_writeerr);
    }

    ga_clear(&ga);
    FreeWild(filecount, files);

#else
    // No language support, just use "*.txt" and "tags".
    helptags_one(dirname, (char_u *)".txt", (char_u *)"tags", add_help_tags,
							    ignore_writeerr);
#endif
}