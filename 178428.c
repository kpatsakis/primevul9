reg_getline_submatch(linenr_T lnum)
{
    char_u *s;
    linenr_T save_first = rex.reg_firstlnum;
    linenr_T save_max = rex.reg_maxline;

    rex.reg_firstlnum = rsm.sm_firstlnum;
    rex.reg_maxline = rsm.sm_maxline;

    s = reg_getline(lnum);

    rex.reg_firstlnum = save_first;
    rex.reg_maxline = save_max;
    return s;
}