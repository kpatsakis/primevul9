set_bm_skip(UChar* s, UChar* end, OnigEncoding enc ARG_UNUSED,
            UChar skip[], int** int_skip)
{
  int i, len;

  len = (int )(end - s);
  if (len < ONIG_CHAR_TABLE_SIZE) {
    for (i = 0; i < ONIG_CHAR_TABLE_SIZE; i++) skip[i] = len;

    for (i = 0; i < len - 1; i++)
      skip[s[i]] = len - 1 - i;
  }
  else {
    if (IS_NULL(*int_skip)) {
      *int_skip = (int* )xmalloc(sizeof(int) * ONIG_CHAR_TABLE_SIZE);
      if (IS_NULL(*int_skip)) return ONIGERR_MEMORY;
    }
    for (i = 0; i < ONIG_CHAR_TABLE_SIZE; i++) (*int_skip)[i] = len;

    for (i = 0; i < len - 1; i++)
      (*int_skip)[s[i]] = len - 1 - i;
  }
  return 0;
}