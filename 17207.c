ptr2cells(char_u *p)
{
    // For UTF-8 we need to look at more bytes if the first byte is >= 0x80.
    if (enc_utf8 && *p >= 0x80)
	return utf_ptr2cells(p);
    // For DBCS we can tell the cell count from the first byte.
    return (g_chartab[*p] & CT_CELL_MASK);
}