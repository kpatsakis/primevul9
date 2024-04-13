onig_get_callout_num_by_tag(regex_t* reg,
                            const UChar* tag, const UChar* tag_end)
{
  int r;
  RegexExt* ext;
  CalloutTagVal e;

  ext = REG_EXTP(reg);
  if (IS_NULL(ext) || IS_NULL(ext->tag_table))
    return ONIGERR_INVALID_CALLOUT_TAG_NAME;

  r = onig_st_lookup_strend(ext->tag_table, tag, tag_end,
                            (HashDataType* )((void* )(&e)));
  if (r == 0) return ONIGERR_INVALID_CALLOUT_TAG_NAME;
  return (int )e;
}