ref_param_write_name_value(const gs_memory_t *mem, ref * pref, const gs_param_string * pvalue)
{
    return name_ref(mem, pvalue->data, pvalue->size, pref,
                    (pvalue->persistent ? 0 : 1));
}