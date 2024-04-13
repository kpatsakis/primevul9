ansi_color2rgb(int nr, char_u *r, char_u *g, char_u *b, char_u *ansi_idx)
{
    if (nr < 16)
    {
	*r = ansi_table[nr][0];
	*g = ansi_table[nr][1];
	*b = ansi_table[nr][2];
	*ansi_idx = nr;
    }
    else
    {
	*r = 0;
	*g = 0;
	*b = 0;
	*ansi_idx = ANSI_INDEX_NONE;
    }
}