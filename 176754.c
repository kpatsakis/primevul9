static void wsrep_sst_complete(THD *thd, int const rcode) {
  Wsrep_client_service client_service(thd, thd->wsrep_cs());
  Wsrep_server_state::instance().sst_received(client_service, rcode,
                                              &sst_awaiting_callback);
}