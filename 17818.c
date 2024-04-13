tv_get_bool(typval_T *varp)
{
    return tv_get_bool_or_number_chk(varp, NULL, TRUE);
}