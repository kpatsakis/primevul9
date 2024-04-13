onig_get_callout_tag_end(regex_t* reg, int callout_num)
{
  CalloutListEntry* e = onig_reg_callout_list_at(reg, callout_num);
  return e->tag_end;
}