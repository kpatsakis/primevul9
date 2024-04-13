S_invlist_is_iterating(SV* const invlist)
{
    PERL_ARGS_ASSERT_INVLIST_IS_ITERATING;

    return *(get_invlist_iter_addr(invlist)) < (STRLEN) UV_MAX;
}