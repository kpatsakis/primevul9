onig_get_regex_ext(regex_t* reg)
{
  if (IS_NULL(REG_EXTP(reg))) {
    RegexExt* ext = (RegexExt* )xmalloc(sizeof(*ext));
    if (IS_NULL(ext)) return 0;

    ext->pattern      = 0;
    ext->pattern_end  = 0;
#ifdef USE_CALLOUT
    ext->tag_table    = 0;
    ext->callout_num  = 0;
    ext->callout_list_alloc = 0;
    ext->callout_list = 0;
#endif

    REG_EXTPL(reg) = (void* )ext;
  }

  return REG_EXTP(reg);
}