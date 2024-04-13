vim_isfilec(int c)
{
    return (c >= 0x100 || (c > 0 && (g_chartab[c] & CT_FNAME_CHAR)));
}