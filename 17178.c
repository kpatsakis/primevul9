vim_isupper(int c)
{
    if (c <= '@')
	return FALSE;
    if (c >= 0x80)
    {
	if (enc_utf8)
	    return utf_isupper(c);
	if (c >= 0x100)
	{
#ifdef HAVE_ISWUPPER
	    if (has_mbyte)
		return iswupper(c);
#endif
	    // islower() can't handle these chars and may crash
	    return FALSE;
	}
	if (enc_latin1like)
	    return (latin1flags[c] & LATIN1UPPER) == LATIN1UPPER;
    }
    return isupper(c);
}