backslash_trans(int c)
{
    switch (c)
    {
	case 'r':   return CAR;
	case 't':   return TAB;
	case 'e':   return ESC;
	case 'b':   return BS;
    }
    return c;
}