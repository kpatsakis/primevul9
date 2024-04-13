tv_get_string_strict(typval_T *varp)
{
    static char_u   mybuf[NUMBUFLEN];
    char_u	    *res =  tv_get_string_buf_chk_strict(
						 varp, mybuf, in_vim9script());

    return res != NULL ? res : (char_u *)"";
}