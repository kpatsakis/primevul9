vim_regexec(regmatch_T *rmp, char_u *line, colnr_T col)
{
    return vim_regexec_string(rmp, line, col, FALSE);
}