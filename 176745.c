void wsrep_sst_received(THD *thd, const wsrep_uuid_t &uuid,
                        wsrep_seqno_t const seqno,
                        const void *const state __attribute__((unused)),
                        size_t const state_len __attribute__((unused))) {
  /*
    To keep track of whether the local uuid:seqno should be updated. Also, note
    that local state (uuid:seqno) is updated/checkpointed only after we get an
    OK from wsrep provider. By doing so, the values remain consistent across
    the server & wsrep provider.
  */
  /*
    TODO: Handle backwards compatibility. WSREP API v25 does not have
    wsrep schema.
  */
  /*
    Logical SST methods (mysqldump etc) don't update InnoDB sys header.
    Reset the SE checkpoint before recovering view in order to avoid
    sanity check failure.
   */
  wsrep::gtid const sst_gtid(wsrep::id(uuid.data, sizeof(uuid.data)),
                             wsrep::seqno(seqno));

  if (!wsrep_before_SE()) {
    wsrep_set_SE_checkpoint(wsrep::gtid::undefined());
    wsrep_set_SE_checkpoint(sst_gtid);
  }
  wsrep_verify_SE_checkpoint(uuid, seqno);

  /*
    Both wsrep_init_SR() and wsrep_recover_view() may use
    wsrep thread pool. Restore original thd context before returning.
  */
  if (thd) {
    wsrep_store_threadvars(thd);
  }

  if (WSREP_ON) {
    int const rcode(seqno < 0 ? seqno : 0);
    wsrep_sst_complete(thd, rcode);
  }
}