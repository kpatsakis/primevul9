find_start_rawstring(int ind_maxcomment)	// XXX
{
    pos_T	*pos;
    int		cur_maxcomment = ind_maxcomment;

    for (;;)
    {
	pos = findmatchlimit(NULL, 'R', FM_BACKWARD, cur_maxcomment);
	if (pos == NULL)
	    break;

	// Check if the raw string start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	if (!is_pos_in_string(ml_get(pos->lnum), pos->col))
	    break;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	if (cur_maxcomment <= 0)
	{
	    pos = NULL;
	    break;
	}
    }
    return pos;
}