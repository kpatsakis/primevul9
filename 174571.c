compile_unletlock(char_u *arg, exarg_T *eap, cctx_T *cctx)
{
    int	    deep = 0;
    char_u  *p = arg;

    if (eap->cmdidx != CMD_unlet)
    {
	if (eap->forceit)
	    deep = -1;
	else if (vim_isdigit(*p))
	{
	    deep = getdigits(&p);
	    p = skipwhite(p);
	}
	else
	    deep = 2;
    }

    ex_unletlock(eap, p, deep, GLV_NO_AUTOLOAD | GLV_COMPILING,
	    eap->cmdidx == CMD_unlet ? compile_unlet : compile_lock_unlock,
	    cctx);
    return eap->nextcmd == NULL ? (char_u *)"" : eap->nextcmd;
}