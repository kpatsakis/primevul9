S_invlist_clone(pTHX_ SV* const invlist)
{

    /* Return a new inversion list that is a copy of the input one, which is
     * unchanged.  The new list will not be mortal even if the old one was. */

    /* Need to allocate extra space to accommodate Perl's addition of a
     * trailing NUL to SvPV's, since it thinks they are always strings */
    SV* new_invlist = _new_invlist(_invlist_len(invlist) + 1);
    STRLEN physical_length = SvCUR(invlist);
    bool offset = *(get_invlist_offset_addr(invlist));

    PERL_ARGS_ASSERT_INVLIST_CLONE;

    *(get_invlist_offset_addr(new_invlist)) = offset;
    invlist_set_len(new_invlist, _invlist_len(invlist), offset);
    Copy(SvPVX(invlist), SvPVX(new_invlist), physical_length, char);

    return new_invlist;
}