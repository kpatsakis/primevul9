S_invlist_set_previous_index(SV* const invlist, const IV index)
{
    /* Caches <index> for later retrieval */

    PERL_ARGS_ASSERT_INVLIST_SET_PREVIOUS_INDEX;

    assert(index == 0 || index < (int) _invlist_len(invlist));

    *get_invlist_previous_index_addr(invlist) = index;
}