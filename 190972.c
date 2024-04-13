call_func_retnr(
    char_u      *func,
    int		argc,
    typval_T	*argv)
{
    typval_T	rettv;
    varnumber_T	retval;

    if (call_vim_function(func, argc, argv, &rettv) == FAIL)
	return -1;

    retval = tv_get_number_chk(&rettv, NULL);
    clear_tv(&rettv);
    return retval;
}