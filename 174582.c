check_global_and_subst(char_u *cmd, char_u *arg)
{
    if (arg == cmd + 1 && vim_strchr((char_u *)":-.", *arg) != NULL)
    {
	semsg(_(e_separator_not_supported_str), arg);
	return FAIL;
    }
    if (VIM_ISWHITE(cmd[1]))
    {
	semsg(_(e_no_white_space_allowed_before_separator_str), cmd);
	return FAIL;
    }
    return OK;
}