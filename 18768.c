termcapinit(char_u *name)
{
    char_u	*term;

    if (name != NULL && *name == NUL)
	name = NULL;	    // empty name is equal to no name
    term = name;

#ifndef MSWIN
    if (term == NULL)
	term = mch_getenv((char_u *)"TERM");
#endif
    if (term == NULL || *term == NUL)
	term = DEFAULT_TERM;
    set_string_option_direct((char_u *)"term", -1, term, OPT_FREE, 0);

    // Set the default terminal name.
    set_string_default("term", term);
    set_string_default("ttytype", term);

    /*
     * Avoid using "term" here, because the next mch_getenv() may overwrite it.
     */
    set_termname(T_NAME != NULL ? T_NAME : term);
}