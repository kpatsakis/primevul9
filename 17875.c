tv_get_string_buf_chk(typval_T *varp, char_u *buf)
{
    return tv_get_string_buf_chk_strict(varp, buf, FALSE);
}