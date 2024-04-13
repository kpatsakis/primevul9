f_readblob(typval_T *argvars, typval_T *rettv)
{
    if (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	return;

    read_file_or_blob(argvars, rettv, TRUE);
}