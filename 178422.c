make_extmatch(void)
{
    reg_extmatch_T	*em;

    em = ALLOC_CLEAR_ONE(reg_extmatch_T);
    if (em != NULL)
	em->refcnt = 1;
    return em;
}