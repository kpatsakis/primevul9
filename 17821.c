tv_stringify(typval_T *varp, char_u *buf)
{
    if (varp->v_type == VAR_LIST
	    || varp->v_type == VAR_DICT
	    || varp->v_type == VAR_BLOB
	    || varp->v_type == VAR_FUNC
	    || varp->v_type == VAR_PARTIAL
	    || varp->v_type == VAR_FLOAT)
    {
	typval_T tmp;

	f_string(varp, &tmp);
	tv_get_string_buf(&tmp, buf);
	clear_tv(varp);
	*varp = tmp;
	return tmp.vval.v_string;
    }
    return tv_get_string_buf(varp, buf);
}