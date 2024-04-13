measured_bw_line_parse(measured_bw_line_t *out, const char *orig_line)
{
  char *line = tor_strdup(orig_line);
  char *cp = line;
  int got_bw = 0;
  int got_node_id = 0;
  char *strtok_state; /* lame sauce d'jour */
  cp = tor_strtok_r(cp, " \t", &strtok_state);

  if (!cp) {
    log_warn(LD_DIRSERV, "Invalid line in bandwidth file: %s",
             escaped(orig_line));
    tor_free(line);
    return -1;
  }

  if (orig_line[strlen(orig_line)-1] != '\n') {
    log_warn(LD_DIRSERV, "Incomplete line in bandwidth file: %s",
             escaped(orig_line));
    tor_free(line);
    return -1;
  }

  do {
    if (strcmpstart(cp, "bw=") == 0) {
      int parse_ok = 0;
      char *endptr;
      if (got_bw) {
        log_warn(LD_DIRSERV, "Double bw= in bandwidth file line: %s",
                 escaped(orig_line));
        tor_free(line);
        return -1;
      }
      cp+=strlen("bw=");

      out->bw_kb = tor_parse_long(cp, 0, 0, LONG_MAX, &parse_ok, &endptr);
      if (!parse_ok || (*endptr && !TOR_ISSPACE(*endptr))) {
        log_warn(LD_DIRSERV, "Invalid bandwidth in bandwidth file line: %s",
                 escaped(orig_line));
        tor_free(line);
        return -1;
      }
      got_bw=1;
    } else if (strcmpstart(cp, "node_id=$") == 0) {
      if (got_node_id) {
        log_warn(LD_DIRSERV, "Double node_id= in bandwidth file line: %s",
                 escaped(orig_line));
        tor_free(line);
        return -1;
      }
      cp+=strlen("node_id=$");

      if (strlen(cp) != HEX_DIGEST_LEN ||
          base16_decode(out->node_id, DIGEST_LEN,
                        cp, HEX_DIGEST_LEN) != DIGEST_LEN) {
        log_warn(LD_DIRSERV, "Invalid node_id in bandwidth file line: %s",
                 escaped(orig_line));
        tor_free(line);
        return -1;
      }
      strlcpy(out->node_hex, cp, sizeof(out->node_hex));
      got_node_id=1;
    }
  } while ((cp = tor_strtok_r(NULL, " \t", &strtok_state)));

  if (got_bw && got_node_id) {
    tor_free(line);
    return 0;
  } else {
    log_warn(LD_DIRSERV, "Incomplete line in bandwidth file: %s",
             escaped(orig_line));
    tor_free(line);
    return -1;
  }
}