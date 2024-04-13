dirserv_read_guardfraction_file_from_str(const char *guardfraction_file_str,
                                      smartlist_t *vote_routerstatuses)
{
  config_line_t *front=NULL, *line;
  int ret_tmp;
  int retval = -1;
  int current_line_n = 0; /* line counter for better log messages */

  /* Guardfraction info to be parsed */
  int total_consensuses = 0;
  int total_days = 0;

  /* Stats */
  int guards_read_n = 0;
  int guards_applied_n = 0;

  /* Parse file and split it in lines */
  ret_tmp = config_get_lines(guardfraction_file_str, &front, 0);
  if (ret_tmp < 0) {
    log_warn(LD_CONFIG, "Error reading from guardfraction file");
    goto done;
  }

  /* Sort routerstatuses (needed later when applying guardfraction info) */
  if (vote_routerstatuses)
    smartlist_sort(vote_routerstatuses, compare_vote_routerstatus_entries);

  for (line = front; line; line=line->next) {
    current_line_n++;

    if (!strcmp(line->key, GUARDFRACTION_VERSION)) {
      int num_ok = 1;
      unsigned int version;

      version =
        (unsigned int) tor_parse_long(line->value,
                                      10, 0, INT_MAX, &num_ok, NULL);

      if (!num_ok || version != 1) {
        log_warn(LD_GENERAL, "Got unknown guardfraction version %d.", version);
        goto done;
      }
    } else if (!strcmp(line->key, GUARDFRACTION_DATE_STR)) {
      time_t file_written_at;
      time_t now = time(NULL);

      /* First line is 'written-at <date>' */
      if (parse_iso_time(line->value, &file_written_at) < 0) {
        log_warn(LD_CONFIG, "Guardfraction:%d: Bad date '%s'. Ignoring",
                 current_line_n, line->value);
        goto done; /* don't tolerate failure here. */
      }
      if (file_written_at < now - MAX_GUARDFRACTION_FILE_AGE) {
        log_warn(LD_CONFIG, "Guardfraction:%d: was written very long ago '%s'",
                 current_line_n, line->value);
        goto done; /* don't tolerate failure here. */
      }
    } else if (!strcmp(line->key, GUARDFRACTION_INPUTS)) {
      char *err_msg = NULL;

      if (guardfraction_file_parse_inputs_line(line->value,
                                               &total_consensuses,
                                               &total_days,
                                               &err_msg) < 0) {
        log_warn(LD_CONFIG, "Guardfraction:%d: %s",
                 current_line_n, err_msg);
        tor_free(err_msg);
        continue;
      }

    } else if (!strcmp(line->key, GUARDFRACTION_GUARD)) {
      char *err_msg = NULL;

      ret_tmp = guardfraction_file_parse_guard_line(line->value,
                                                    vote_routerstatuses,
                                                    &err_msg);
      if (ret_tmp < 0) { /* failed while parsing the guard line */
        log_warn(LD_CONFIG, "Guardfraction:%d: %s",
                 current_line_n, err_msg);
        tor_free(err_msg);
        continue;
      }

      /* Successfully parsed guard line. Check if it was applied properly. */
      guards_read_n++;
      if (ret_tmp > 0) {
        guards_applied_n++;
      }
    } else {
      log_warn(LD_CONFIG, "Unknown guardfraction line %d (%s %s)",
               current_line_n, line->key, line->value);
    }
  }

  retval = 0;

  log_info(LD_CONFIG,
           "Successfully parsed guardfraction file with %d consensuses over "
           "%d days. Parsed %d nodes and applied %d of them%s.",
           total_consensuses, total_days, guards_read_n, guards_applied_n,
           vote_routerstatuses ? "" : " (no routerstatus provided)" );

 done:
  config_free_lines(front);

  if (retval < 0) {
    return retval;
  } else {
    return guards_read_n;
  }
}