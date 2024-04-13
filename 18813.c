term_set_winsize(int height, int width)
{
    OUT_STR(tgoto((char *)T_CWS, width, height));
}