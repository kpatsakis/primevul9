re_mult_next(char *what)
{
    if (re_multi_type(peekchr()) == MULTI_MULT)
    {
       semsg(_(e_nfa_regexp_cannot_repeat_str), what);
       rc_did_emsg = TRUE;
       return FAIL;
    }
    return OK;
}