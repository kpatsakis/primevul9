read_file_or_blob(typval_T *argvars, typval_T *rettv, int always_blob)
{
    int		binary = FALSE;
    int		blob = always_blob;
    int		failed = FALSE;
    char_u	*fname;
    FILE	*fd;
    char_u	buf[(IOSIZE/256)*256];	// rounded to avoid odd + 1
    int		io_size = sizeof(buf);
    int		readlen;		// size of last fread()
    char_u	*prev	 = NULL;	// previously read bytes, if any
    long	prevlen  = 0;		// length of data in prev
    long	prevsize = 0;		// size of prev buffer
    long	maxline  = MAXLNUM;
    long	cnt	 = 0;
    char_u	*p;			// position in buf
    char_u	*start;			// start of current line

    if (argvars[1].v_type != VAR_UNKNOWN)
    {
	if (STRCMP(tv_get_string(&argvars[1]), "b") == 0)
	    binary = TRUE;
	if (STRCMP(tv_get_string(&argvars[1]), "B") == 0)
	    blob = TRUE;

	if (argvars[2].v_type != VAR_UNKNOWN)
	    maxline = (long)tv_get_number(&argvars[2]);
    }

    if ((blob ? rettv_blob_alloc(rettv) : rettv_list_alloc(rettv)) == FAIL)
	return;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    fname = tv_get_string(&argvars[0]);

    if (mch_isdir(fname))
    {
	semsg(_(e_src_is_directory), fname);
	return;
    }
    if (*fname == NUL || (fd = mch_fopen((char *)fname, READBIN)) == NULL)
    {
	semsg(_(e_cant_open_file_str), *fname == NUL ? (char_u *)_("<empty>") : fname);
	return;
    }

    if (blob)
    {
	if (read_blob(fd, rettv->vval.v_blob) == FAIL)
	{
	    semsg(_(e_cant_read_file_str), fname);
	    // An empty blob is returned on error.
	    blob_free(rettv->vval.v_blob);
	    rettv->vval.v_blob = NULL;
	}
	fclose(fd);
	return;
    }

    while (cnt < maxline || maxline < 0)
    {
	readlen = (int)fread(buf, 1, io_size, fd);

	// This for loop processes what was read, but is also entered at end
	// of file so that either:
	// - an incomplete line gets written
	// - a "binary" file gets an empty line at the end if it ends in a
	//   newline.
	for (p = buf, start = buf;
		p < buf + readlen || (readlen <= 0 && (prevlen > 0 || binary));
		++p)
	{
	    if (readlen <= 0 || *p == '\n')
	    {
		listitem_T  *li;
		char_u	    *s	= NULL;
		long_u	    len = p - start;

		// Finished a line.  Remove CRs before NL.
		if (readlen > 0 && !binary)
		{
		    while (len > 0 && start[len - 1] == '\r')
			--len;
		    // removal may cross back to the "prev" string
		    if (len == 0)
			while (prevlen > 0 && prev[prevlen - 1] == '\r')
			    --prevlen;
		}
		if (prevlen == 0)
		    s = vim_strnsave(start, len);
		else
		{
		    // Change "prev" buffer to be the right size.  This way
		    // the bytes are only copied once, and very long lines are
		    // allocated only once.
		    if ((s = vim_realloc(prev, prevlen + len + 1)) != NULL)
		    {
			mch_memmove(s + prevlen, start, len);
			s[prevlen + len] = NUL;
			prev = NULL; // the list will own the string
			prevlen = prevsize = 0;
		    }
		}
		if (s == NULL)
		{
		    do_outofmem_msg((long_u) prevlen + len + 1);
		    failed = TRUE;
		    break;
		}

		if ((li = listitem_alloc()) == NULL)
		{
		    vim_free(s);
		    failed = TRUE;
		    break;
		}
		li->li_tv.v_type = VAR_STRING;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_string = s;
		list_append(rettv->vval.v_list, li);

		start = p + 1; // step over newline
		if ((++cnt >= maxline && maxline >= 0) || readlen <= 0)
		    break;
	    }
	    else if (*p == NUL)
		*p = '\n';
	    // Check for utf8 "bom"; U+FEFF is encoded as EF BB BF.  Do this
	    // when finding the BF and check the previous two bytes.
	    else if (*p == 0xbf && enc_utf8 && !binary)
	    {
		// Find the two bytes before the 0xbf.	If p is at buf, or buf
		// + 1, these may be in the "prev" string.
		char_u back1 = p >= buf + 1 ? p[-1]
				     : prevlen >= 1 ? prev[prevlen - 1] : NUL;
		char_u back2 = p >= buf + 2 ? p[-2]
			  : p == buf + 1 && prevlen >= 1 ? prev[prevlen - 1]
			  : prevlen >= 2 ? prev[prevlen - 2] : NUL;

		if (back2 == 0xef && back1 == 0xbb)
		{
		    char_u *dest = p - 2;

		    // Usually a BOM is at the beginning of a file, and so at
		    // the beginning of a line; then we can just step over it.
		    if (start == dest)
			start = p + 1;
		    else
		    {
			// have to shuffle buf to close gap
			int adjust_prevlen = 0;

			if (dest < buf)
			{
			    // must be 1 or 2
			    adjust_prevlen = (int)(buf - dest);
			    dest = buf;
			}
			if (readlen > p - buf + 1)
			    mch_memmove(dest, p + 1, readlen - (p - buf) - 1);
			readlen -= 3 - adjust_prevlen;
			prevlen -= adjust_prevlen;
			p = dest - 1;
		    }
		}
	    }
	} // for

	if (failed || (cnt >= maxline && maxline >= 0) || readlen <= 0)
	    break;
	if (start < p)
	{
	    // There's part of a line in buf, store it in "prev".
	    if (p - start + prevlen >= prevsize)
	    {
		// need bigger "prev" buffer
		char_u *newprev;

		// A common use case is ordinary text files and "prev" gets a
		// fragment of a line, so the first allocation is made
		// small, to avoid repeatedly 'allocing' large and
		// 'reallocing' small.
		if (prevsize == 0)
		    prevsize = (long)(p - start);
		else
		{
		    long grow50pc = (prevsize * 3) / 2;
		    long growmin  = (long)((p - start) * 2 + prevlen);
		    prevsize = grow50pc > growmin ? grow50pc : growmin;
		}
		newprev = vim_realloc(prev, prevsize);
		if (newprev == NULL)
		{
		    do_outofmem_msg((long_u)prevsize);
		    failed = TRUE;
		    break;
		}
		prev = newprev;
	    }
	    // Add the line part to end of "prev".
	    mch_memmove(prev + prevlen, start, p - start);
	    prevlen += (long)(p - start);
	}
    } // while

    // For a negative line count use only the lines at the end of the file,
    // free the rest.
    if (!failed && maxline < 0)
	while (cnt > -maxline)
	{
	    listitem_remove(rettv->vval.v_list, rettv->vval.v_list->lv_first);
	    --cnt;
	}

    if (failed)
    {
	// an empty list is returned on error
	list_free(rettv->vval.v_list);
	rettv_list_alloc(rettv);
    }

    vim_free(prev);
    fclose(fd);
}