ref_param_write_typed(gs_param_list * plist, gs_param_name pkey,
                      gs_param_typed_value * pvalue)
{
    iparam_list *const iplist = (iparam_list *) plist;
    ref value;
    int code = 0;

    switch (pvalue->type) {
        case gs_param_type_null:
            make_null(&value);
            break;
        case gs_param_type_bool:
            make_bool(&value, pvalue->value.b);
            break;
        case gs_param_type_int:
            make_int(&value, pvalue->value.i);
            break;
        case gs_param_type_long:
            make_int(&value, pvalue->value.l);
            break;
        case gs_param_type_float:
            make_real(&value, pvalue->value.f);
            break;
        case gs_param_type_string:
            if (!ref_param_requested(plist, pkey))
                return 0;
            code = ref_param_write_string_value(&value, &pvalue->value.s,
                                                iplist->ref_memory);
            break;
        case gs_param_type_name:
            if (!ref_param_requested(plist, pkey))
                return 0;
            code = ref_param_write_name_value(iplist->memory, &value, &pvalue->value.n);
            break;
        case gs_param_type_int_array:
            return ref_param_write_typed_array(plist, pkey, &pvalue->value.ia,
                                               pvalue->value.ia.size,
                                               ref_param_make_int);
        case gs_param_type_float_array:
            return ref_param_write_typed_array(plist, pkey, &pvalue->value.fa,
                                               pvalue->value.fa.size,
                                               ref_param_make_float);
        case gs_param_type_string_array:
            return ref_param_write_typed_array(plist, pkey, &pvalue->value.sa,
                                               pvalue->value.sa.size,
                                               ref_param_make_string);
        case gs_param_type_name_array:
            return ref_param_write_typed_array(plist, pkey, &pvalue->value.na,
                                               pvalue->value.na.size,
                                               ref_param_make_name);
        case gs_param_type_dict:
        case gs_param_type_dict_int_keys:
        case gs_param_type_array:
            return ref_param_begin_write_collection(plist, pkey,
                                                    &pvalue->value.d,
              (gs_param_collection_type_t)(pvalue->type - gs_param_type_dict));
        default:
            return_error(gs_error_typecheck);
    }
    if (code < 0)
        return code;
    return ref_param_write(iplist, pkey, &value);
}