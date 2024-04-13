onig_search(regex_t* reg, const UChar* str, const UChar* end,
            const UChar* start, const UChar* range, OnigRegion* region,
            OnigOptionType option)
{
  int r;
  OnigMatchParam mp;

  onig_initialize_match_param(&mp);
  r = onig_search_with_param(reg, str, end, start, range, region, option, &mp);
  onig_free_match_param_content(&mp);
  return r;

}