dirserv_read_measured_bandwidths(const char *from_file,
                                 smartlist_t *routerstatuses)
{
  char line[512];
  FILE *fp = tor_fopen_cloexec(from_file, "r");
  int applied_lines = 0;
  time_t file_time, now;
  int ok;

  if (fp == NULL) {
    log_warn(LD_CONFIG, "Can't open bandwidth file at configured location: %s",
             from_file);
    return -1;
  }

  if (!fgets(line, sizeof(line), fp)
          || !strlen(line) || line[strlen(line)-1] != '\n') {
    log_warn(LD_DIRSERV, "Long or truncated time in bandwidth file: %s",
             escaped(line));
    fclose(fp);
    return -1;
  }

  line[strlen(line)-1] = '\0';
  file_time = (time_t)tor_parse_ulong(line, 10, 0, ULONG_MAX, &ok, NULL);
  if (!ok) {
    log_warn(LD_DIRSERV, "Non-integer time in bandwidth file: %s",
             escaped(line));
    fclose(fp);
    return -1;
  }

  now = time(NULL);
  if ((now - file_time) > MAX_MEASUREMENT_AGE) {
    log_warn(LD_DIRSERV, "Bandwidth measurement file stale. Age: %u",
             (unsigned)(time(NULL) - file_time));
    fclose(fp);
    return -1;
  }

  if (routerstatuses)
    smartlist_sort(routerstatuses, compare_vote_routerstatus_entries);

  while (!feof(fp)) {
    measured_bw_line_t parsed_line;
    if (fgets(line, sizeof(line), fp) && strlen(line)) {
      if (measured_bw_line_parse(&parsed_line, line) != -1) {
        /* Also cache the line for dirserv_get_bandwidth_for_router() */
        dirserv_cache_measured_bw(&parsed_line, file_time);
        if (measured_bw_line_apply(&parsed_line, routerstatuses) > 0)
          applied_lines++;
      }
    }
  }

  /* Now would be a nice time to clean the cache, too */
  dirserv_expire_measured_bw_cache(now);

  fclose(fp);
  log_info(LD_DIRSERV,
           "Bandwidth measurement file successfully read. "
           "Applied %d measurements.", applied_lines);
  return 0;
}