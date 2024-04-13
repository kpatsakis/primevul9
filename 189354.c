file_ff_differs(buf_T *buf, int ignore_empty)
{
    /* In a buffer that was never loaded the options are not valid. */
    if (buf->b_flags & BF_NEVERLOADED)
	return FALSE;
    if (ignore_empty
	    && (buf->b_flags & BF_NEW)
	    && buf->b_ml.ml_line_count == 1
	    && *ml_get_buf(buf, (linenr_T)1, FALSE) == NUL)
	return FALSE;
    if (buf->b_start_ffc != *buf->b_p_ff)
	return TRUE;
    if ((buf->b_p_bin || !buf->b_p_fixeol) && buf->b_start_eol != buf->b_p_eol)
	return TRUE;
#ifdef FEAT_MBYTE
    if (!buf->b_p_bin && buf->b_start_bomb != buf->b_p_bomb)
	return TRUE;
    if (buf->b_start_fenc == NULL)
	return (*buf->b_p_fenc != NUL);
    return (STRCMP(buf->b_start_fenc, buf->b_p_fenc) != 0);
#else
    return FALSE;
#endif
}