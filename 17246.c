char2cells(int c)
{
    if (IS_SPECIAL(c))
	return char2cells(K_SECOND(c)) + 2;
    if (c >= 0x80)
    {
	// UTF-8: above 0x80 need to check the value
	if (enc_utf8)
	    return utf_char2cells(c);
	// DBCS: double-byte means double-width, except for euc-jp with first
	// byte 0x8e
	if (enc_dbcs != 0 && c >= 0x100)
	{
	    if (enc_dbcs == DBCS_JPNU && ((unsigned)c >> 8) == 0x8e)
		return 1;
	    return 2;
	}
    }
    return (g_chartab[c & 0xff] & CT_CELL_MASK);
}