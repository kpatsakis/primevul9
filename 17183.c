vim_toupper(int c)
{
    if (c <= '@')
	return c;
    if (c >= 0x80 || !(cmp_flags & CMP_KEEPASCII))
    {
	if (enc_utf8)
	    return utf_toupper(c);
	if (c >= 0x100)
	{
#ifdef HAVE_TOWUPPER
	    if (has_mbyte)
		return towupper(c);
#endif
	    // toupper() can't handle these chars and may crash
	    return c;
	}
	if (enc_latin1like)
	    return latin1upper[c];
    }
    if (c < 0x80 && (cmp_flags & CMP_KEEPASCII))
	return TOUPPER_ASC(c);
    return TOUPPER_LOC(c);
}