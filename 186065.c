slow_search_ic(OnigEncoding enc, int case_fold_flag,
               UChar* target, UChar* target_end,
               const UChar* text, const UChar* text_end, UChar* text_range)
{
  UChar *s, *end;

  end = (UChar* )text_end;
  end -= target_end - target - 1;
  if (end > text_range)
    end = text_range;

  s = (UChar* )text;

  while (s < end) {
    if (str_lower_case_match(enc, case_fold_flag, target, target_end,
                             s, text_end))
      return s;

    s += enclen(enc, s);
  }

  return (UChar* )NULL;
}