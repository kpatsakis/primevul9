is_all_code_len_1_items(int n, OnigCaseFoldCodeItem items[])
{
  int i;

  for (i = 0; i < n; i++) {
    OnigCaseFoldCodeItem* item = items + i;
    if (item->code_len != 1) return 0;
  }

  return 1;
}