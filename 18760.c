stuff_inserted(
    int	    c,		// Command character to be inserted
    long    count,	// Repeat this many times
    int	    no_esc)	// Don't add an ESC at the end
{
    char_u	*esc_ptr;
    char_u	*ptr;
    char_u	*last_ptr;
    char_u	last = NUL;

    ptr = get_last_insert();
    if (ptr == NULL)
    {
	emsg(_(e_no_inserted_text_yet));
	return FAIL;
    }

    // may want to stuff the command character, to start Insert mode
    if (c != NUL)
	stuffcharReadbuff(c);
    if ((esc_ptr = (char_u *)vim_strrchr(ptr, ESC)) != NULL)
	*esc_ptr = NUL;	    // remove the ESC

    // when the last char is either "0" or "^" it will be quoted if no ESC
    // comes after it OR if it will inserted more than once and "ptr"
    // starts with ^D.	-- Acevedo
    last_ptr = (esc_ptr ? esc_ptr : ptr + STRLEN(ptr)) - 1;
    if (last_ptr >= ptr && (*last_ptr == '0' || *last_ptr == '^')
	    && (no_esc || (*ptr == Ctrl_D && count > 1)))
    {
	last = *last_ptr;
	*last_ptr = NUL;
    }

    do
    {
	stuffReadbuff(ptr);
	// a trailing "0" is inserted as "<C-V>048", "^" as "<C-V>^"
	if (last)
	    stuffReadbuff((char_u *)(last == '0'
			? IF_EB("\026\060\064\070", CTRL_V_STR "xf0")
			: IF_EB("\026^", CTRL_V_STR "^")));
    }
    while (--count > 0);

    if (last)
	*last_ptr = last;

    if (esc_ptr != NULL)
	*esc_ptr = ESC;	    // put the ESC back

    // may want to stuff a trailing ESC, to get out of Insert mode
    if (!no_esc)
	stuffcharReadbuff(ESC);

    return OK;
}