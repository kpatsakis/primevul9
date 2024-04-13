byte2cells(int b)
{
    if (enc_utf8 && b >= 0x80)
	return 0;
    return (g_chartab[b] & CT_CELL_MASK);
}