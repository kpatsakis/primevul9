bool wsrep_before_SE() {
  return (wsrep_provider != NULL && strcmp(wsrep_provider, WSREP_NONE) &&
          strcmp(wsrep_sst_method, WSREP_SST_SKIP));
}