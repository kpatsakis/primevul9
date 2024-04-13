vim_islower(int c)
{
    if (c <= '@')
	return FALSE;
    if (c >= 0x80)
    {
	if (enc_utf8)
	    return utf_islower(c);
	if (c >= 0x100)
	{
#ifdef HAVE_ISWLOWER
	    if (has_mbyte)
		return iswlower(c);
#endif
	    // islower() can't handle these chars and may crash
	    return FALSE;
	}
	if (enc_latin1like)
	    return (latin1flags[c] & LATIN1LOWER) == LATIN1LOWER;
    }
    return islower(c);
}