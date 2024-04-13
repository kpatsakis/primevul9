tv_get_number_chk(typval_T *varp, int *denote)
{
    return tv_get_bool_or_number_chk(varp, denote, FALSE);
}