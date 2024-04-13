tv_get_buf_from_arg(typval_T *tv)
{
    buf_T *buf;

    ++emsg_off;
    buf = tv_get_buf(tv, FALSE);
    --emsg_off;
    if (buf == NULL
	    && tv->v_type != VAR_NUMBER
	    && tv->v_type != VAR_STRING)
	// issue errmsg for type error
	(void)tv_get_number(tv);
    return buf;
}