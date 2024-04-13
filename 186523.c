back_in_line(void)
{
    int		sclass;		    // starting class

    sclass = cls();
    for (;;)
    {
	if (curwin->w_cursor.col == 0)	    // stop at start of line
	    break;
	dec_cursor();
	if (cls() != sclass)		    // stop at start of word
	{
	    inc_cursor();
	    break;
	}
    }
}