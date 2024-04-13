S_invlist_previous_index(SV* const invlist)
{
    /* Returns cached index of previous search */

    PERL_ARGS_ASSERT_INVLIST_PREVIOUS_INDEX;

    return *get_invlist_previous_index_addr(invlist);
}