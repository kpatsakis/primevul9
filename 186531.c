skip_chars(int cclass, int dir)
{
    while (cls() == cclass)
	if ((dir == FORWARD ? inc_cursor() : dec_cursor()) == -1)
	    return TRUE;
    return FALSE;
}