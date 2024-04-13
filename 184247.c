free_regex_ext(RegexExt* ext)
{
  if (IS_NOT_NULL(ext)) {
    if (IS_NOT_NULL(ext->pattern))
      xfree((void* )ext->pattern);

#ifdef USE_CALLOUT
    if (IS_NOT_NULL(ext->tag_table))
      onig_callout_tag_table_free(ext->tag_table);

    if (IS_NOT_NULL(ext->callout_list))
      onig_free_reg_callout_list(ext->callout_num, ext->callout_list);
#endif

    xfree(ext);
  }
}