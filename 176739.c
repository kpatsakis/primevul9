bool wsrep_sst_method_check(sys_var *, THD *, set_var *var) {
  if ((!var->save_result.string_value.str) ||
      (var->save_result.string_value.length == 0)) {
    my_error(ER_WRONG_VALUE_FOR_VAR, MYF(0), var->var->name.str,
             var->save_result.string_value.str
                 ? var->save_result.string_value.str
                 : "NULL");
    return true;
  }

  if (strcmp(var->save_result.string_value.str, WSREP_SST_XTRABACKUP_V2) != 0) {
    my_error(ER_WRONG_VALUE_FOR_VAR, MYF(0), var->var->name.str,
             var->save_result.string_value.str
                 ? var->save_result.string_value.str
                 : "NULL");
    return true;
  }

  return false;
}