f_glob2regpat(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*pat;

    if (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	return;

    pat = tv_get_string_buf_chk_strict(&argvars[0], buf, in_vim9script());
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = (pat == NULL)
			 ? NULL : file_pat_to_reg_pat(pat, NULL, NULL, FALSE);
}