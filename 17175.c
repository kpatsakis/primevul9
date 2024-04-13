vim_iswordp_buf(char_u *p, buf_T *buf)
{
    int	c = *p;

    if (has_mbyte && MB_BYTE2LEN(c) > 1)
	c = (*mb_ptr2char)(p);
    return vim_iswordc_buf(c, buf);
}