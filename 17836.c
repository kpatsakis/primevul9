tv_check_lock(typval_T *tv, char_u *name, int use_gettext)
{
    int	lock = 0;

    switch (tv->v_type)
    {
	case VAR_BLOB:
	    if (tv->vval.v_blob != NULL)
		lock = tv->vval.v_blob->bv_lock;
	    break;
	case VAR_LIST:
	    if (tv->vval.v_list != NULL)
		lock = tv->vval.v_list->lv_lock;
	    break;
	case VAR_DICT:
	    if (tv->vval.v_dict != NULL)
		lock = tv->vval.v_dict->dv_lock;
	    break;
	default:
	    break;
    }
    return value_check_lock(tv->v_lock, name, use_gettext)
		   || (lock != 0 && value_check_lock(lock, name, use_gettext));
}