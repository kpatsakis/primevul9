tv_get_lnum_buf(typval_T *argvars, buf_T *buf)
{
    if (argvars[0].v_type == VAR_STRING
	    && argvars[0].vval.v_string != NULL
	    && argvars[0].vval.v_string[0] == '$'
	    && argvars[0].vval.v_string[1] == NUL
	    && buf != NULL)
	return buf->b_ml.ml_line_count;
    return (linenr_T)tv_get_number_chk(&argvars[0], NULL);
}