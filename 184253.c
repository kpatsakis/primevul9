get_min_max_byte_len_case_fold_items(int n, OnigCaseFoldCodeItem items[],
                                     OnigLen* rmin, OnigLen* rmax)
{
  int i;
  OnigLen len, minlen, maxlen;

  minlen = INFINITE_LEN;
  maxlen = 0;
  for (i = 0; i < n; i++) {
    OnigCaseFoldCodeItem* item = items + i;

    len = item->byte_len;
    if (len < minlen) minlen = len;
    if (len > maxlen) maxlen = len;
  }

  *rmin = minlen;
  *rmax = maxlen;
  return 0;
}