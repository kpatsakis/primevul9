guardfraction_file_parse_inputs_line(const char *inputs_line,
                                     int *total_consensuses,
                                     int *total_days,
                                     char **err_msg)
{
  int retval = -1;
  char *inputs_tmp = NULL;
  int num_ok = 1;
  smartlist_t *sl = smartlist_new();

  tor_assert(err_msg);

  /* Second line is inputs information:
   *   n-inputs <total_consensuses> <total_days>. */
  smartlist_split_string(sl, inputs_line, " ",
                         SPLIT_SKIP_SPACE|SPLIT_IGNORE_BLANK, 3);
  if (smartlist_len(sl) < 2) {
    tor_asprintf(err_msg, "incomplete line '%s'", inputs_line);
    goto done;
  }

  inputs_tmp = smartlist_get(sl, 0);
  *total_consensuses =
    (int) tor_parse_long(inputs_tmp, 10, 0, INT_MAX, &num_ok, NULL);
  if (!num_ok) {
    tor_asprintf(err_msg, "unparseable consensus '%s'", inputs_tmp);
    goto done;
  }

  inputs_tmp = smartlist_get(sl, 1);
  *total_days =
    (int) tor_parse_long(inputs_tmp, 10, 0, INT_MAX, &num_ok, NULL);
  if (!num_ok) {
    tor_asprintf(err_msg, "unparseable days '%s'", inputs_tmp);
    goto done;
  }

  retval = 0;

 done:
  SMARTLIST_FOREACH(sl, char *, cp, tor_free(cp));
  smartlist_free(sl);

  return retval;
}