onig_callout_tag_is_exist_at_callout_num(regex_t* reg, int callout_num)
{
  RegexExt* ext = REG_EXTP(reg);

  if (IS_NULL(ext) || IS_NULL(ext->callout_list)) return 0;
  if (callout_num > ext->callout_num) return 0;

  return (ext->callout_list[callout_num].flag &
          CALLOUT_TAG_LIST_FLAG_TAG_EXIST) != 0  ? 1 : 0;
}