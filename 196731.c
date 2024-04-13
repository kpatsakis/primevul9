to_name_const_end(char_u *arg)
{
    char_u	*p = arg;
    typval_T	rettv;

    if (STRNCMP(p, "<SNR>", 5) == 0)
	p = skipdigits(p + 5);
    p = to_name_end(p, TRUE);
    if (p == arg && *arg == '[')
    {

	// Can be "[1, 2, 3]->Func()".
	if (eval_list(&p, &rettv, NULL, FALSE) == FAIL)
	    p = arg;
    }
    return p;
}