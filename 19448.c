find_start_of_word(pos_T *pos)
{
    char_u	*line;
    int		cclass;
    int		col;

    line = ml_get(pos->lnum);
    cclass = get_mouse_class(line + pos->col);

    while (pos->col > 0)
    {
	col = pos->col - 1;
	col -= (*mb_head_off)(line, line + col);
	if (get_mouse_class(line + col) != cclass)
	    break;
	pos->col = col;
    }
}