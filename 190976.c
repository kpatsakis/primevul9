call_func_noret(
    char_u      *func,
    int		argc,
    typval_T	*argv)
{
    typval_T	rettv;

    if (call_vim_function(func, argc, argv, &rettv) == FAIL)
	return FAIL;
    clear_tv(&rettv);
    return OK;
}