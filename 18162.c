copy_text_attr(
    int		off,
    char_u	*buf,
    int		len,
    int		attr)
{
    int		i;

    mch_memmove(ScreenLines + off, buf, (size_t)len);
    if (enc_utf8)
	vim_memset(ScreenLinesUC + off, 0, sizeof(u8char_T) * (size_t)len);
    for (i = 0; i < len; ++i)
	ScreenAttrs[off + i] = attr;
}