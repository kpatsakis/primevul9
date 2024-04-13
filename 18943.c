networkstatus_v2_parse_from_string(const char *s)
{
  const char *eos, *s_dup = s;
  smartlist_t *tokens = smartlist_create();
  smartlist_t *footer_tokens = smartlist_create();
  networkstatus_v2_t *ns = NULL;
  char ns_digest[DIGEST_LEN];
  char tmp_digest[DIGEST_LEN];
  struct in_addr in;
  directory_token_t *tok;
  int i;
  memarea_t *area = NULL;

  if (router_get_networkstatus_v2_hash(s, ns_digest)) {
    log_warn(LD_DIR, "Unable to compute digest of network-status");
    goto err;
  }

  area = memarea_new();
  eos = find_start_of_next_routerstatus(s);
  if (tokenize_string(area, s, eos, tokens, netstatus_token_table,0)) {
    log_warn(LD_DIR, "Error tokenizing network-status header.");
    goto err;
  }
  ns = tor_malloc_zero(sizeof(networkstatus_v2_t));
  memcpy(ns->networkstatus_digest, ns_digest, DIGEST_LEN);

  tok = find_by_keyword(tokens, K_NETWORK_STATUS_VERSION);
  tor_assert(tok->n_args >= 1);
  if (strcmp(tok->args[0], "2")) {
    log_warn(LD_BUG, "Got a non-v2 networkstatus. Version was "
             "%s", escaped(tok->args[0]));
    goto err;
  }

  tok = find_by_keyword(tokens, K_DIR_SOURCE);
  tor_assert(tok->n_args >= 3);
  ns->source_address = tor_strdup(tok->args[0]);
  if (tor_inet_aton(tok->args[1], &in) == 0) {
    log_warn(LD_DIR, "Error parsing network-status source address %s",
             escaped(tok->args[1]));
    goto err;
  }
  ns->source_addr = ntohl(in.s_addr);
  ns->source_dirport =
    (uint16_t) tor_parse_long(tok->args[2],10,0,65535,NULL,NULL);
  if (ns->source_dirport == 0) {
    log_warn(LD_DIR, "Directory source without dirport; skipping.");
    goto err;
  }

  tok = find_by_keyword(tokens, K_FINGERPRINT);
  tor_assert(tok->n_args);
  if (base16_decode(ns->identity_digest, DIGEST_LEN, tok->args[0],
                    strlen(tok->args[0]))) {
    log_warn(LD_DIR, "Couldn't decode networkstatus fingerprint %s",
             escaped(tok->args[0]));
    goto err;
  }

  if ((tok = find_opt_by_keyword(tokens, K_CONTACT))) {
    tor_assert(tok->n_args);
    ns->contact = tor_strdup(tok->args[0]);
  }

  tok = find_by_keyword(tokens, K_DIR_SIGNING_KEY);
  tor_assert(tok->key);
  ns->signing_key = tok->key;
  tok->key = NULL;

  if (crypto_pk_get_digest(ns->signing_key, tmp_digest)<0) {
    log_warn(LD_DIR, "Couldn't compute signing key digest");
    goto err;
  }
  if (tor_memneq(tmp_digest, ns->identity_digest, DIGEST_LEN)) {
    log_warn(LD_DIR,
             "network-status fingerprint did not match dir-signing-key");
    goto err;
  }

  if ((tok = find_opt_by_keyword(tokens, K_DIR_OPTIONS))) {
    for (i=0; i < tok->n_args; ++i) {
      if (!strcmp(tok->args[i], "Names"))
        ns->binds_names = 1;
      if (!strcmp(tok->args[i], "Versions"))
        ns->recommends_versions = 1;
      if (!strcmp(tok->args[i], "BadExits"))
        ns->lists_bad_exits = 1;
      if (!strcmp(tok->args[i], "BadDirectories"))
        ns->lists_bad_directories = 1;
    }
  }

  if (ns->recommends_versions) {
    if (!(tok = find_opt_by_keyword(tokens, K_CLIENT_VERSIONS))) {
      log_warn(LD_DIR, "Missing client-versions on versioning directory");
      goto err;
    }
    ns->client_versions = tor_strdup(tok->args[0]);

    if (!(tok = find_opt_by_keyword(tokens, K_SERVER_VERSIONS)) ||
        tok->n_args<1) {
      log_warn(LD_DIR, "Missing server-versions on versioning directory");
      goto err;
    }
    ns->server_versions = tor_strdup(tok->args[0]);
  }

  tok = find_by_keyword(tokens, K_PUBLISHED);
  tor_assert(tok->n_args == 1);
  if (parse_iso_time(tok->args[0], &ns->published_on) < 0) {
     goto err;
  }

  ns->entries = smartlist_create();
  s = eos;
  SMARTLIST_FOREACH(tokens, directory_token_t *, t, token_clear(t));
  smartlist_clear(tokens);
  memarea_clear(area);
  while (!strcmpstart(s, "r ")) {
    routerstatus_t *rs;
    if ((rs = routerstatus_parse_entry_from_string(area, &s, tokens,
                                                   NULL, NULL, 0, 0)))
      smartlist_add(ns->entries, rs);
  }
  smartlist_sort(ns->entries, compare_routerstatus_entries);
  smartlist_uniq(ns->entries, compare_routerstatus_entries,
                 _free_duplicate_routerstatus_entry);

  if (tokenize_string(area,s, NULL, footer_tokens, dir_footer_token_table,0)) {
    log_warn(LD_DIR, "Error tokenizing network-status footer.");
    goto err;
  }
  if (smartlist_len(footer_tokens) < 1) {
    log_warn(LD_DIR, "Too few items in network-status footer.");
    goto err;
  }
  tok = smartlist_get(footer_tokens, smartlist_len(footer_tokens)-1);
  if (tok->tp != K_DIRECTORY_SIGNATURE) {
    log_warn(LD_DIR,
             "Expected network-status footer to end with a signature.");
    goto err;
  }

  note_crypto_pk_op(VERIFY_DIR);
  if (check_signature_token(ns_digest, DIGEST_LEN, tok, ns->signing_key, 0,
                            "network-status") < 0)
    goto err;

  goto done;
 err:
  dump_desc(s_dup, "v2 networkstatus");
  networkstatus_v2_free(ns);
  ns = NULL;
 done:
  SMARTLIST_FOREACH(tokens, directory_token_t *, t, token_clear(t));
  smartlist_free(tokens);
  SMARTLIST_FOREACH(footer_tokens, directory_token_t *, t, token_clear(t));
  smartlist_free(footer_tokens);
  if (area) {
    DUMP_AREA(area, "v2 networkstatus");
    memarea_drop_all(area);
  }
  return ns;
}