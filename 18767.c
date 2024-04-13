term_7to8bit(char_u *p)
{
    if (*p == ESC)
    {
	if (p[1] == '[')
	    return CSI;
	if (p[1] == ']')
	    return OSC;
	if (p[1] == 'O')
	    return 0x8f;
    }
    return 0;
}