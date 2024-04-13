cls(void)
{
    int	    c;

    c = gchar_cursor();
    if (c == ' ' || c == '\t' || c == NUL)
	return 0;
    if (enc_dbcs != 0 && c > 0xFF)
    {
	// If cls_bigword, report multi-byte chars as class 1.
	if (enc_dbcs == DBCS_KOR && cls_bigword)
	    return 1;

	// process code leading/trailing bytes
	return dbcs_class(((unsigned)c >> 8), (c & 0xFF));
    }
    if (enc_utf8)
    {
	c = utf_class(c);
	if (c != 0 && cls_bigword)
	    return 1;
	return c;
    }

    // If cls_bigword is TRUE, report all non-blanks as class 1.
    if (cls_bigword)
	return 1;

    if (vim_iswordc(c))
	return 2;
    return 1;
}