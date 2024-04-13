put_string_in_typebuf(
	int	offset,
	int	slen,
	char_u	*string,
	int	new_slen,
	char_u	*buf,
	int	bufsize,
	int	*buflen)
{
    int		extra = new_slen - slen;

    string[new_slen] = NUL;
    if (buf == NULL)
    {
	if (extra < 0)
	    // remove matched chars, taking care of noremap
	    del_typebuf(-extra, offset);
	else if (extra > 0)
	    // insert the extra space we need
	    if (ins_typebuf(string + slen, REMAP_YES, offset, FALSE, FALSE)
								       == FAIL)
		return FAIL;

	// Careful: del_typebuf() and ins_typebuf() may have reallocated
	// typebuf.tb_buf[]!
	mch_memmove(typebuf.tb_buf + typebuf.tb_off + offset, string,
							     (size_t)new_slen);
    }
    else
    {
	if (extra < 0)
	    // remove matched characters
	    mch_memmove(buf + offset, buf + offset - extra,
					   (size_t)(*buflen + offset + extra));
	else if (extra > 0)
	{
	    // Insert the extra space we need.  If there is insufficient
	    // space return -1.
	    if (*buflen + extra + new_slen >= bufsize)
		return FAIL;
	    mch_memmove(buf + offset + extra, buf + offset,
						   (size_t)(*buflen - offset));
	}
	mch_memmove(buf + offset, string, (size_t)new_slen);
	*buflen = *buflen + extra + new_slen;
    }
    return OK;
}