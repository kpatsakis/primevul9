sunday_quick_search_case_fold(regex_t* reg,
                              const UChar* target, const UChar* target_end,
                              const UChar* text,   const UChar* text_end,
                              const UChar* text_range)
{
  const UChar *s, *se, *end;
  const UChar *tail;
  int skip, tlen1;
  int map_offset;
  int case_fold_flag;
  OnigEncoding enc;

#ifdef ONIG_DEBUG_SEARCH
  fprintf(stderr,
          "sunday_quick_search_case_fold: text: %p, text_end: %p, text_range: %p\n", text, text_end, text_range);
#endif

  enc = reg->enc;
  case_fold_flag = reg->case_fold_flag;

  tail = target_end - 1;
  tlen1 = (int )(tail - target);
  end = text_range;
  if (end + tlen1 > text_end)
    end = text_end - tlen1;

  map_offset = reg->map_offset;
  s = text;

  while (s < end) {
    if (str_lower_case_match(enc, case_fold_flag, target, target_end,
                             s, text_end))
      return (UChar* )s;

    se = s + tlen1;
    if (se + map_offset >= text_end) break;
    skip = reg->map[*(se + map_offset)];
#if 0
    p = s;
    do {
      s += enclen(enc, s);
    } while ((s - p) < skip && s < end);
#else
    /* This is faster than prev code for long text.  ex: /(?i)Twain/  */
    s += skip;
    if (s < end)
      s = onigenc_get_right_adjust_char_head(enc, text, s);
#endif
  }

  return (UChar* )NULL;
}