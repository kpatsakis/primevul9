command_process_relay_cell(cell_t *cell, or_connection_t *conn)
{
  circuit_t *circ;
  int reason, direction;

  circ = circuit_get_by_circid_orconn(cell->circ_id, conn);

  if (!circ) {
    log_debug(LD_OR,
              "unknown circuit %d on connection from %s:%d. Dropping.",
              cell->circ_id, conn->_base.address, conn->_base.port);
    return;
  }

  if (circ->state == CIRCUIT_STATE_ONIONSKIN_PENDING) {
    log_fn(LOG_PROTOCOL_WARN,LD_PROTOCOL,"circuit in create_wait. Closing.");
    circuit_mark_for_close(circ, END_CIRC_REASON_TORPROTOCOL);
    return;
  }

  if (CIRCUIT_IS_ORIGIN(circ)) {
    /* if we're a relay and treating connections with recent local
     * traffic better, then this is one of them. */
    conn->client_used = time(NULL);
  }

  if (!CIRCUIT_IS_ORIGIN(circ) &&
      cell->circ_id == TO_OR_CIRCUIT(circ)->p_circ_id)
    direction = CELL_DIRECTION_OUT;
  else
    direction = CELL_DIRECTION_IN;

  /* If we have a relay_early cell, make sure that it's outbound, and we've
   * gotten no more than MAX_RELAY_EARLY_CELLS_PER_CIRCUIT of them. */
  if (cell->command == CELL_RELAY_EARLY) {
    if (direction == CELL_DIRECTION_IN) {
      /* XXX Allow an unlimited number of inbound relay_early cells for
       * now, for hidden service compatibility. See bug 1038. -RD */
    } else {
      or_circuit_t *or_circ = TO_OR_CIRCUIT(circ);
      if (or_circ->remaining_relay_early_cells == 0) {
        log_fn(LOG_PROTOCOL_WARN, LD_OR,
               "Received too many RELAY_EARLY cells on circ %d from %s:%d."
               "  Closing circuit.",
               cell->circ_id, safe_str(conn->_base.address), conn->_base.port);
        circuit_mark_for_close(circ, END_CIRC_REASON_TORPROTOCOL);
        return;
      }
      --or_circ->remaining_relay_early_cells;
    }
  }

  if ((reason = circuit_receive_relay_cell(cell, circ, direction)) < 0) {
    log_fn(LOG_PROTOCOL_WARN,LD_PROTOCOL,"circuit_receive_relay_cell "
           "(%s) failed. Closing.",
           direction==CELL_DIRECTION_OUT?"forward":"backward");
    circuit_mark_for_close(circ, -reason);
  }
}