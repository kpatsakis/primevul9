command_process_cell(cell_t *cell, or_connection_t *conn)
{
  int handshaking = (conn->_base.state == OR_CONN_STATE_OR_HANDSHAKING);
#ifdef KEEP_TIMING_STATS
  /* how many of each cell have we seen so far this second? needs better
   * name. */
  static int num_create=0, num_created=0, num_relay=0, num_destroy=0;
  /* how long has it taken to process each type of cell? */
  static int create_time=0, created_time=0, relay_time=0, destroy_time=0;
  static time_t current_second = 0; /* from previous calls to time */

  time_t now = time(NULL);

  if (now > current_second) { /* the second has rolled over */
    /* print stats */
    log_info(LD_OR,
         "At end of second: %d creates (%d ms), %d createds (%d ms), "
         "%d relays (%d ms), %d destroys (%d ms)",
         num_create, create_time/1000,
         num_created, created_time/1000,
         num_relay, relay_time/1000,
         num_destroy, destroy_time/1000);

    /* zero out stats */
    num_create = num_created = num_relay = num_destroy = 0;
    create_time = created_time = relay_time = destroy_time = 0;

    /* remember which second it is, for next time */
    current_second = now;
  }
#endif

#ifdef KEEP_TIMING_STATS
#define PROCESS_CELL(tp, cl, cn) STMT_BEGIN {                   \
    ++num ## tp;                                                \
    command_time_process_cell(cl, cn, & tp ## time ,            \
                              command_process_ ## tp ## _cell);  \
  } STMT_END
#else
#define PROCESS_CELL(tp, cl, cn) command_process_ ## tp ## _cell(cl, cn)
#endif

  /* Reject all but VERSIONS and NETINFO when handshaking. */
  if (handshaking && cell->command != CELL_VERSIONS &&
      cell->command != CELL_NETINFO)
    return;

  switch (cell->command) {
    case CELL_PADDING:
      ++stats_n_padding_cells_processed;
      /* do nothing */
      break;
    case CELL_CREATE:
    case CELL_CREATE_FAST:
      ++stats_n_create_cells_processed;
      PROCESS_CELL(create, cell, conn);
      break;
    case CELL_CREATED:
    case CELL_CREATED_FAST:
      ++stats_n_created_cells_processed;
      PROCESS_CELL(created, cell, conn);
      break;
    case CELL_RELAY:
    case CELL_RELAY_EARLY:
      ++stats_n_relay_cells_processed;
      PROCESS_CELL(relay, cell, conn);
      break;
    case CELL_DESTROY:
      ++stats_n_destroy_cells_processed;
      PROCESS_CELL(destroy, cell, conn);
      break;
    case CELL_VERSIONS:
      tor_fragile_assert();
      break;
    case CELL_NETINFO:
      ++stats_n_netinfo_cells_processed;
      PROCESS_CELL(netinfo, cell, conn);
      break;
    default:
      log_fn(LOG_INFO, LD_PROTOCOL,
             "Cell of unknown type (%d) received. Dropping.", cell->command);
      break;
  }
}