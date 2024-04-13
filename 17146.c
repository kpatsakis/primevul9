vim_iswordc_buf(int c, buf_T *buf)
{
    if (c >= 0x100)
    {
	if (enc_dbcs != 0)
	    return dbcs_class((unsigned)c >> 8, (unsigned)(c & 0xff)) >= 2;
	if (enc_utf8)
	    return utf_class_buf(c, buf) >= 2;
	return FALSE;
    }
    return (c > 0 && GET_CHARTAB(buf, c) != 0);
}