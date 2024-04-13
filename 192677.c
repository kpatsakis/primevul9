S__invlist_array_init(SV* const invlist, const bool will_have_0)
{
    /* Returns a pointer to the first element in the inversion list's array.
     * This is called upon initialization of an inversion list.  Where the
     * array begins depends on whether the list has the code point U+0000 in it
     * or not.  The other parameter tells it whether the code that follows this
     * call is about to put a 0 in the inversion list or not.  The first
     * element is either the element reserved for 0, if TRUE, or the element
     * after it, if FALSE */

    bool* offset = get_invlist_offset_addr(invlist);
    UV* zero_addr = (UV *) SvPVX(invlist);

    PERL_ARGS_ASSERT__INVLIST_ARRAY_INIT;

    /* Must be empty */
    assert(! _invlist_len(invlist));

    *zero_addr = 0;

    /* 1^1 = 0; 1^0 = 1 */
    *offset = 1 ^ will_have_0;
    return zero_addr + *offset;
}