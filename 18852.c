find_first_tcap(
    char_u *name,
    int	    code)
{
    struct builtin_term *p;

    for (p = find_builtin_term(name); p->bt_string != NULL; ++p)
	if (p->bt_entry == code)
	    return p;
    return NULL;
}