vim_isprintc(int c)
{
    if (enc_utf8 && c >= 0x100)
	return utf_printable(c);
    return (c >= 0x100 || (c > 0 && (g_chartab[c] & CT_PRINT_CHAR)));
}