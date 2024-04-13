tv_get_bool_chk(typval_T *varp, int *denote)
{
    return tv_get_bool_or_number_chk(varp, denote, TRUE);
}