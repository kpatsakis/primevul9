f_terminalprops(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_TERMRESPONSE
    int i;
# endif

    if (rettv_dict_alloc(rettv) == FAIL)
	return;
# ifdef FEAT_TERMRESPONSE
    for (i = 0; i < TPR_COUNT; ++i)
    {
	char_u	value[2];

	value[0] = term_props[i].tpr_status;
	value[1] = NUL;
	dict_add_string(rettv->vval.v_dict, term_props[i].tpr_name, value);
    }
# endif
}