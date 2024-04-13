f_filereadable(typval_T *argvars, typval_T *rettv)
{
    int		fd;
    char_u	*p;
    int		n;

    if (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	return;

#ifndef O_NONBLOCK
# define O_NONBLOCK 0
#endif
    p = tv_get_string(&argvars[0]);
    if (*p && !mch_isdir(p) && (fd = mch_open((char *)p,
					      O_RDONLY | O_NONBLOCK, 0)) >= 0)
    {
	n = TRUE;
	close(fd);
    }
    else
	n = FALSE;

    rettv->vval.v_number = n;
}