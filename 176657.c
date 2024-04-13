getline_peek_skip_comments(evalarg_T *evalarg)
{
    for (;;)
    {
	char_u *next = getline_peek(evalarg->eval_getline,
							 evalarg->eval_cookie);
	char_u *p;

	if (next == NULL)
	    break;
	p = skipwhite(next);
	if (*p != NUL && !vim9_comment_start(p))
	    return next;
	if (eval_next_line(evalarg) == NULL)
	    break;
    }
    return NULL;
}