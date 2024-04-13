router_parse_runningrouters(const char *str)
{
  char digest[DIGEST_LEN];
  directory_token_t *tok;
  time_t published_on;
  int r = -1;
  crypto_pk_env_t *declared_key = NULL;
  smartlist_t *tokens = NULL;
  const char *eos = str + strlen(str), *str_dup = str;
  memarea_t *area = NULL;

  if (router_get_runningrouters_hash(str, digest)) {
    log_warn(LD_DIR, "Unable to compute digest of running-routers");
    goto err;
  }
  area = memarea_new();
  tokens = smartlist_create();
  if (tokenize_string(area,str,eos,tokens,dir_token_table,0)) {
    log_warn(LD_DIR, "Error tokenizing running-routers"); goto err;
  }
  tok = smartlist_get(tokens,0);
  if (tok->tp != K_NETWORK_STATUS) {
    log_warn(LD_DIR, "Network-status starts with wrong token");
    goto err;
  }

  tok = find_by_keyword(tokens, K_PUBLISHED);
  tor_assert(tok->n_args == 1);
  if (parse_iso_time(tok->args[0], &published_on) < 0) {
     goto err;
  }
  if (!(tok = find_opt_by_keyword(tokens, K_DIRECTORY_SIGNATURE))) {
    log_warn(LD_DIR, "Missing signature on running-routers");
    goto err;
  }
  declared_key = find_dir_signing_key(str, eos);
  note_crypto_pk_op(VERIFY_DIR);
  if (check_signature_token(digest, DIGEST_LEN, tok, declared_key,
                            CST_CHECK_AUTHORITY, "running-routers")
      < 0)
    goto err;

  /* Now that we know the signature is okay, and we have a
   * publication time, cache the list. */
  if (get_options()->DirPort && !authdir_mode_v1(get_options()))
    dirserv_set_cached_directory(str, published_on, 1);

  r = 0;
 err:
  dump_desc(str_dup, "v1 running-routers");
  if (declared_key) crypto_free_pk_env(declared_key);
  if (tokens) {
    SMARTLIST_FOREACH(tokens, directory_token_t *, t, token_clear(t));
    smartlist_free(tokens);
  }
  if (area) {
    DUMP_AREA(area, "v1 running-routers");
    memarea_drop_all(area);
  }
  return r;
}