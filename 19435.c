find_end_of_word(pos_T *pos)
{
    char_u	*line;
    int		cclass;
    int		col;

    line = ml_get(pos->lnum);
    if (*p_sel == 'e' && pos->col > 0)
    {
	--pos->col;
	pos->col -= (*mb_head_off)(line, line + pos->col);
    }
    cclass = get_mouse_class(line + pos->col);
    while (line[pos->col] != NUL)
    {
	col = pos->col + (*mb_ptr2len)(line + pos->col);
	if (get_mouse_class(line + col) != cclass)
	{
	    if (*p_sel == 'e')
		pos->col = col;
	    break;
	}
	pos->col = col;
    }
}