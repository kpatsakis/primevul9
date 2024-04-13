vim_regexec_prog(
    regprog_T	**prog,
    int		ignore_case,
    char_u	*line,
    colnr_T	col)
{
    int		r;
    regmatch_T	regmatch;

    regmatch.regprog = *prog;
    regmatch.rm_ic = ignore_case;
    r = vim_regexec_string(&regmatch, line, col, FALSE);
    *prog = regmatch.regprog;
    return r;
}