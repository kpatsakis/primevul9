tv_get_string_buf_chk_strict(typval_T *varp, char_u *buf, int strict)
{
    switch (varp->v_type)
    {
	case VAR_NUMBER:
	    if (strict)
	    {
		emsg(_(e_using_number_as_string));
		break;
	    }
	    vim_snprintf((char *)buf, NUMBUFLEN, "%lld",
					    (varnumber_T)varp->vval.v_number);
	    return buf;
	case VAR_FUNC:
	case VAR_PARTIAL:
	    emsg(_(e_using_funcref_as_string));
	    break;
	case VAR_LIST:
	    emsg(_(e_using_list_as_string));
	    break;
	case VAR_DICT:
	    emsg(_(e_using_dictionary_as_string));
	    break;
	case VAR_FLOAT:
#ifdef FEAT_FLOAT
	    if (strict)
	    {
		emsg(_(e_using_float_as_string));
		break;
	    }
	    vim_snprintf((char *)buf, NUMBUFLEN, "%g", varp->vval.v_float);
	    return buf;
#endif
	case VAR_STRING:
	    if (varp->vval.v_string != NULL)
		return varp->vval.v_string;
	    return (char_u *)"";
	case VAR_BOOL:
	case VAR_SPECIAL:
	    STRCPY(buf, get_var_special_name(varp->vval.v_number));
	    return buf;
	case VAR_BLOB:
	    emsg(_(e_using_blob_as_string));
	    break;
	case VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    if (in_vim9script())
	    {
		semsg(_(e_using_invalid_value_as_string_str), "job");
		break;
	    }
	    return job_to_string_buf(varp, buf);
#endif
	    break;
	case VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    if (in_vim9script())
	    {
		semsg(_(e_using_invalid_value_as_string_str), "channel");
		break;
	    }
	    return channel_to_string_buf(varp, buf);
#endif
	    break;
	case VAR_VOID:
	    emsg(_(e_cannot_use_void_value));
	    break;
	case VAR_UNKNOWN:
	case VAR_ANY:
	case VAR_INSTR:
	    semsg(_(e_using_invalid_value_as_string_str),
						  vartype_name(varp->v_type));
	    break;
    }
    return NULL;
}