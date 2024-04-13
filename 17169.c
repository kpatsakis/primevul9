win_linetabsize(win_T *wp, char_u *line, colnr_T len)
{
    colnr_T	col = 0;
    char_u	*s;

    for (s = line; *s != NUL && (len == MAXCOL || s < line + len);
								MB_PTR_ADV(s))
	col += win_lbr_chartabsize(wp, line, s, col, NULL);
    return (int)col;
}