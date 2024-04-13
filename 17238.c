expand_wildcards(
    int		   num_pat,	// number of input patterns
    char_u	 **pat,		// array of input patterns
    int		  *num_files,	// resulting number of files
    char_u	***files,	// array of resulting files
    int		   flags)	// EW_DIR, etc.
{
    int		retval;
    int		i, j;
    char_u	*p;
    int		non_suf_match;	// number without matching suffix

    retval = gen_expand_wildcards(num_pat, pat, num_files, files, flags);

    // When keeping all matches, return here
    if ((flags & EW_KEEPALL) || retval == FAIL)
	return retval;

#ifdef FEAT_WILDIGN
    /*
     * Remove names that match 'wildignore'.
     */
    if (*p_wig)
    {
	char_u	*ffname;

	// check all files in (*files)[]
	for (i = 0; i < *num_files; ++i)
	{
	    ffname = FullName_save((*files)[i], FALSE);
	    if (ffname == NULL)		// out of memory
		break;
# ifdef VMS
	    vms_remove_version(ffname);
# endif
	    if (match_file_list(p_wig, (*files)[i], ffname))
	    {
		// remove this matching file from the list
		vim_free((*files)[i]);
		for (j = i; j + 1 < *num_files; ++j)
		    (*files)[j] = (*files)[j + 1];
		--*num_files;
		--i;
	    }
	    vim_free(ffname);
	}

	// If the number of matches is now zero, we fail.
	if (*num_files == 0)
	{
	    VIM_CLEAR(*files);
	    return FAIL;
	}
    }
#endif

    /*
     * Move the names where 'suffixes' match to the end.
     */
    if (*num_files > 1)
    {
	non_suf_match = 0;
	for (i = 0; i < *num_files; ++i)
	{
	    if (!match_suffix((*files)[i]))
	    {
		/*
		 * Move the name without matching suffix to the front
		 * of the list.
		 */
		p = (*files)[i];
		for (j = i; j > non_suf_match; --j)
		    (*files)[j] = (*files)[j - 1];
		(*files)[non_suf_match++] = p;
	    }
	}
    }

    return retval;
}