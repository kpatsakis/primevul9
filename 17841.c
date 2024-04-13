tv_get_string(typval_T *varp)
{
    static char_u   mybuf[NUMBUFLEN];

    return tv_get_string_buf(varp, mybuf);
}