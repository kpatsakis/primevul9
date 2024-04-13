vim_tolower(int c)
{
    if (c <= '@')
	return c;
    if (c >= 0x80 || !(cmp_flags & CMP_KEEPASCII))
    {
	if (enc_utf8)
	    return utf_tolower(c);
	if (c >= 0x100)
	{
#ifdef HAVE_TOWLOWER
	    if (has_mbyte)
		return towlower(c);
#endif
	    // tolower() can't handle these chars and may crash
	    return c;
	}
	if (enc_latin1like)
	    return latin1lower[c];
    }
    if (c < 0x80 && (cmp_flags & CMP_KEEPASCII))
	return TOLOWER_ASC(c);
    return TOLOWER_LOC(c);
}