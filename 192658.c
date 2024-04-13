S_invlist_iterinit(SV* invlist)	/* Initialize iterator for invlist */
{
    PERL_ARGS_ASSERT_INVLIST_ITERINIT;

    *get_invlist_iter_addr(invlist) = 0;
}