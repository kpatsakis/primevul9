onig_get_capture_range_in_callout(OnigCalloutArgs* a, int mem_num, int* begin, int* end)
{
  OnigRegex    reg;
  const UChar* str;
  StackType*   stk_base;
  int i;

  i = mem_num;
  reg = a->regex;
  str = a->string;
  stk_base = a->stk_base;

  if (i > 0) {
    if (a->mem_end_stk[i] != INVALID_STACK_INDEX) {
      if (MEM_STATUS_AT(reg->bt_mem_start, i))
        *begin = (int )(STACK_AT(a->mem_start_stk[i])->u.mem.pstr - str);
      else
        *begin = (int )((UChar* )((void* )a->mem_start_stk[i]) - str);

      *end = (int )((MEM_STATUS_AT(reg->bt_mem_end, i)
                     ? STACK_AT(a->mem_end_stk[i])->u.mem.pstr
                     : (UChar* )((void* )a->mem_end_stk[i])) - str);
    }
    else {
      *begin = *end = ONIG_REGION_NOTPOS;
    }
  }
  else
    return ONIGERR_INVALID_ARGUMENT;

  return ONIG_NORMAL;
}