onig_new_without_alloc(regex_t* reg,
                       const UChar* pattern, const UChar* pattern_end,
                       OnigOptionType option, OnigEncoding enc,
                       OnigSyntaxType* syntax, OnigErrorInfo* einfo)
{
  int r;

  r = onig_reg_init(reg, option, ONIGENC_CASE_FOLD_DEFAULT, enc, syntax);
  if (r != 0) return r;

  r = onig_compile(reg, pattern, pattern_end, einfo);
  return r;
}