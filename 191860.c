format_versions_list(config_line_t *ln)
{
  smartlist_t *versions;
  char *result;
  versions = smartlist_new();
  for ( ; ln; ln = ln->next) {
    smartlist_split_string(versions, ln->value, ",",
                           SPLIT_SKIP_SPACE|SPLIT_IGNORE_BLANK, 0);
  }
  sort_version_list(versions, 1);
  result = smartlist_join_strings(versions,",",0,NULL);
  SMARTLIST_FOREACH(versions,char *,s,tor_free(s));
  smartlist_free(versions);
  return result;
}