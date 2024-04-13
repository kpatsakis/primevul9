onig_ext_set_pattern(regex_t* reg, const UChar* pattern, const UChar* pattern_end)
{
  RegexExt* ext;
  UChar* s;

  ext = onig_get_regex_ext(reg);
  CHECK_NULL_RETURN_MEMERR(ext);

  s = onigenc_strdup(reg->enc, pattern, pattern_end);
  CHECK_NULL_RETURN_MEMERR(s);

  ext->pattern     = s;
  ext->pattern_end = s + (pattern_end - pattern);

  return ONIG_NORMAL;
}