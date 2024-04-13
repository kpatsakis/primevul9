command_process_destroy_cell(cell_t *cell, or_connection_t *conn)
{
  circuit_t *circ;
  int reason;

  circ = circuit_get_by_circid_orconn(cell->circ_id, conn);
  reason = (uint8_t)cell->payload[0];
  if (!circ) {
    log_info(LD_OR,"unknown circuit %d on connection from %s:%d. Dropping.",
             cell->circ_id, conn->_base.address, conn->_base.port);
    return;
  }
  log_debug(LD_OR,"Received for circID %d.",cell->circ_id);

  if (!CIRCUIT_IS_ORIGIN(circ) &&
      cell->circ_id == TO_OR_CIRCUIT(circ)->p_circ_id) {
    /* the destroy came from behind */
    circuit_set_p_circid_orconn(TO_OR_CIRCUIT(circ), 0, NULL);
    circuit_mark_for_close(circ, reason|END_CIRC_REASON_FLAG_REMOTE);
  } else { /* the destroy came from ahead */
    circuit_set_n_circid_orconn(circ, 0, NULL);
    if (CIRCUIT_IS_ORIGIN(circ)) {
      circuit_mark_for_close(circ, reason|END_CIRC_REASON_FLAG_REMOTE);
    } else {
      char payload[1];
      log_debug(LD_OR, "Delivering 'truncated' back.");
      payload[0] = (char)reason;
      relay_send_command_from_edge(0, circ, RELAY_COMMAND_TRUNCATED,
                                   payload, sizeof(payload), NULL);
    }
  }
}