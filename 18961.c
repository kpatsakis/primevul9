sort_version_list(smartlist_t *versions, int remove_duplicates)
{
  smartlist_sort(versions, _compare_tor_version_str_ptr);

  if (remove_duplicates)
    smartlist_uniq(versions, _compare_tor_version_str_ptr, _tor_free);
}