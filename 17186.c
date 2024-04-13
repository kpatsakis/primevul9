trans_characters(
    char_u	*buf,
    int		bufsize)
{
    int		len;		// length of string needing translation
    int		room;		// room in buffer after string
    char_u	*trs;		// translated character
    int		trs_len;	// length of trs[]

    len = (int)STRLEN(buf);
    room = bufsize - len;
    while (*buf != 0)
    {
	// Assume a multi-byte character doesn't need translation.
	if (has_mbyte && (trs_len = (*mb_ptr2len)(buf)) > 1)
	    len -= trs_len;
	else
	{
	    trs = transchar_byte(*buf);
	    trs_len = (int)STRLEN(trs);
	    if (trs_len > 1)
	    {
		room -= trs_len - 1;
		if (room <= 0)
		    return;
		mch_memmove(buf + trs_len, buf + 1, (size_t)len);
	    }
	    mch_memmove(buf, trs, (size_t)trs_len);
	    --len;
	}
	buf += trs_len;
    }
}