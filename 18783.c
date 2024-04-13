check_for_codes_from_term(void)
{
    int		c;

    // If no codes requested or all are answered, no need to wait.
    if (xt_index_out == 0 || xt_index_out == xt_index_in)
	return;

    // Vgetc() will check for and handle any response.
    // Keep calling vpeekc() until we don't get any responses.
    ++no_mapping;
    ++allow_keys;
    for (;;)
    {
	c = vpeekc();
	if (c == NUL)	    // nothing available
	    break;

	// If a response is recognized it's replaced with K_IGNORE, must read
	// it from the input stream.  If there is no K_IGNORE we can't do
	// anything, break here (there might be some responses further on, but
	// we don't want to throw away any typed chars).
	if (c != K_SPECIAL && c != K_IGNORE)
	    break;
	c = vgetc();
	if (c != K_IGNORE)
	{
	    vungetc(c);
	    break;
	}
    }
    --no_mapping;
    --allow_keys;
}