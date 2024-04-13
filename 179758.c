is_pos_in_string(char_u *line, colnr_T col)
{
    char_u *p;

    for (p = line; *p && (colnr_T)(p - line) < col; ++p)
	p = skip_string(p);
    return !((colnr_T)(p - line) <= col);
}