ref_extmatch(reg_extmatch_T *em)
{
    if (em != NULL)
	em->refcnt++;
    return em;
}