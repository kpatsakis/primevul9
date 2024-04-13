clear_ppconst(ppconst_T *ppconst)
{
    int	    i;

    for (i = 0; i < ppconst->pp_used; ++i)
	clear_tv(&ppconst->pp_tv[i]);
    ppconst->pp_used = 0;
}