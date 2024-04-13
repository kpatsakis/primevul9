S_add_cp_to_invlist(pTHX_ SV* invlist, const UV cp) {
    return _add_range_to_invlist(invlist, cp, cp);
}