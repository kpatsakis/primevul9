tv_get_number(typval_T *varp)
{
    int		error = FALSE;

    return tv_get_number_chk(varp, &error);	// return 0L on error
}