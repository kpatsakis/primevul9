addfile(
    garray_T	*gap,
    char_u	*f,	// filename
    int		flags)
{
    char_u	*p;
    int		isdir;
    stat_T	sb;

    // if the file/dir/link doesn't exist, may not add it
    if (!(flags & EW_NOTFOUND) && ((flags & EW_ALLLINKS)
			? mch_lstat((char *)f, &sb) < 0 : mch_getperm(f) < 0))
	return;

#ifdef FNAME_ILLEGAL
    // if the file/dir contains illegal characters, don't add it
    if (vim_strpbrk(f, (char_u *)FNAME_ILLEGAL) != NULL)
	return;
#endif

    isdir = mch_isdir(f);
    if ((isdir && !(flags & EW_DIR)) || (!isdir && !(flags & EW_FILE)))
	return;

    // If the file isn't executable, may not add it.  Do accept directories.
    // When invoked from expand_shellcmd() do not use $PATH.
    if (!isdir && (flags & EW_EXEC)
			     && !mch_can_exe(f, NULL, !(flags & EW_SHELLCMD)))
	return;

    // Make room for another item in the file list.
    if (ga_grow(gap, 1) == FAIL)
	return;

    p = alloc(STRLEN(f) + 1 + isdir);
    if (p == NULL)
	return;

    STRCPY(p, f);
#ifdef BACKSLASH_IN_FILENAME
    slash_adjust(p);
#endif
    /*
     * Append a slash or backslash after directory names if none is present.
     */
#ifndef DONT_ADD_PATHSEP_TO_DIR
    if (isdir && (flags & EW_ADDSLASH))
	add_pathsep(p);
#endif
    ((char_u **)gap->ga_data)[gap->ga_len++] = p;
}