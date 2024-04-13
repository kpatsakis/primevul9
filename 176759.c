static void sst_reject_queries(bool close_conn) {
  WSREP_INFO("Rejecting client queries for the duration of SST.");
  if (true == close_conn) wsrep_close_client_connections(false, false);
}