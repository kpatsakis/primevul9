vim_isprintc_strict(int c)
{
    if (enc_dbcs != 0 && c < 0x100 && MB_BYTE2LEN(c) > 1)
	return FALSE;
    if (enc_utf8 && c >= 0x100)
	return utf_printable(c);
    return (c >= 0x100 || (c > 0 && (g_chartab[c] & CT_PRINT_CHAR)));
}