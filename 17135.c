vim_isIDc(int c)
{
    return (c > 0 && c < 0x100 && (g_chartab[c] & CT_ID_CHAR));
}