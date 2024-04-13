command_process_var_cell(var_cell_t *cell, or_connection_t *conn)
{
#ifdef KEEP_TIMING_STATS
  /* how many of each cell have we seen so far this second? needs better
   * name. */
  static int num_versions=0, num_cert=0;

  time_t now = time(NULL);

  if (now > current_second) { /* the second has rolled over */
    /* print stats */
    log_info(LD_OR,
             "At end of second: %d versions (%d ms), %d cert (%d ms)",
             num_versions, versions_time/1000,
             cert, cert_time/1000);

    num_versions = num_cert = 0;
    versions_time = cert_time = 0;

    /* remember which second it is, for next time */
    current_second = now;
  }
#endif

  /* reject all when not handshaking. */
  if (conn->_base.state != OR_CONN_STATE_OR_HANDSHAKING)
    return;

  switch (cell->command) {
    case CELL_VERSIONS:
      ++stats_n_versions_cells_processed;
      PROCESS_CELL(versions, cell, conn);
      break;
    default:
      log_warn(LD_BUG,
               "Variable-length cell of unknown type (%d) received.",
               cell->command);
      tor_fragile_assert();
      break;
  }
}