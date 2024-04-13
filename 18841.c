term_set_winpos(int x, int y)
{
    // Can't handle a negative value here
    if (x < 0)
	x = 0;
    if (y < 0)
	y = 0;
    OUT_STR(tgoto((char *)T_CWP, y, x));
}