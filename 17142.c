home_replace_save(
    buf_T	*buf,	// when not NULL, check for help files
    char_u	*src)	// input file name
{
    char_u	*dst;
    unsigned	len;

    len = 3;			// space for "~/" and trailing NUL
    if (src != NULL)		// just in case
	len += (unsigned)STRLEN(src);
    dst = alloc(len);
    if (dst != NULL)
	home_replace(buf, src, dst, len, TRUE);
    return dst;
}