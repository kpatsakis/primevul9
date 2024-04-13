f_tempname(typval_T *argvars UNUSED, typval_T *rettv)
{
    static int	x = 'A';

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_tempname(x, FALSE);

    // Advance 'x' to use A-Z and 0-9, so that there are at least 34 different
    // names.  Skip 'I' and 'O', they are used for shell redirection.
    do
    {
	if (x == 'Z')
	    x = '0';
	else if (x == '9')
	    x = 'A';
	else
	    ++x;
    } while (x == 'I' || x == 'O');
}