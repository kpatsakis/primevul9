unref_extmatch(reg_extmatch_T *em)
{
    int i;

    if (em != NULL && --em->refcnt <= 0)
    {
	for (i = 0; i < NSUBEXP; ++i)
	    vim_free(em->matches[i]);
	vim_free(em);
    }
}