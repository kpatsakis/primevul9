bool wsrep_sst_receive_address_check(sys_var *, THD *, set_var *var) {
  char addr_buf[FN_REFLEN];

  if ((!var->save_result.string_value.str) ||
      (var->save_result.string_value.length > (FN_REFLEN - 1)))  // safety
  {
    goto err;
  }

  memcpy(addr_buf, var->save_result.string_value.str,
         var->save_result.string_value.length);
  addr_buf[var->save_result.string_value.length] = 0;

  return false;

err:
  my_error(ER_WRONG_VALUE_FOR_VAR, MYF(0), var->var->name.str,
           var->save_result.string_value.str ? var->save_result.string_value.str
                                             : "NULL");
  return true;
}