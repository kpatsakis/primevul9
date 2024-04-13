tv_get_buf(typval_T *tv, int curtab_only)
{
    char_u	*name = tv->vval.v_string;
    buf_T	*buf;

    if (tv->v_type == VAR_NUMBER)
	return buflist_findnr((int)tv->vval.v_number);
    if (tv->v_type != VAR_STRING)
	return NULL;
    if (name == NULL || *name == NUL)
	return curbuf;
    if (name[0] == '$' && name[1] == NUL)
	return lastbuf;

    buf = buflist_find_by_name(name, curtab_only);

    // If not found, try expanding the name, like done for bufexists().
    if (buf == NULL)
	buf = find_buffer(tv);

    return buf;
}