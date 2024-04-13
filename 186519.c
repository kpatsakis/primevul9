findsent_forward(
    long    count,
    int	    at_start_sent)	// cursor is at start of sentence
{
    while (count--)
    {
	findsent(FORWARD, 1L);
	if (at_start_sent)
	    find_first_blank(&curwin->w_cursor);
	if (count == 0 || at_start_sent)
	    decl(&curwin->w_cursor);
	at_start_sent = !at_start_sent;
    }
}