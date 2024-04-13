tv_get_string_chk(typval_T *varp)
{
    static char_u   mybuf[NUMBUFLEN];

    return tv_get_string_buf_chk(varp, mybuf);
}