hivex_last_modified (hive_h *h)
{
  return timestamp_check (h, 0, h->last_modified);
}