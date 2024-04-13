linetabsize_col(int startcol, char_u *s)
{
    colnr_T	col = startcol;
    char_u	*line = s; // pointer to start of line, for breakindent

    while (*s != NUL)
	col += lbr_chartabsize_adv(line, &s, col);
    return (int)col;
}