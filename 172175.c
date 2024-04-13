onig_reg_callout_list_at(regex_t* reg, int num)
{
  RegexExt* ext = REG_EXTP(reg);
  CHECK_NULL_RETURN(ext);

  if (num <= 0 || num > ext->callout_num)
    return 0;

  num--;
  return ext->callout_list + num;
}