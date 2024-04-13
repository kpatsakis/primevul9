extrainfo_parse_entry_from_string(const char *s, const char *end,
                           int cache_copy, struct digest_ri_map_t *routermap)
{
  extrainfo_t *extrainfo = NULL;
  char digest[128];
  smartlist_t *tokens = NULL;
  directory_token_t *tok;
  crypto_pk_env_t *key = NULL;
  routerinfo_t *router = NULL;
  memarea_t *area = NULL;
  const char *s_dup = s;

  if (!end) {
    end = s + strlen(s);
  }

  /* point 'end' to a point immediately after the final newline. */
  while (end > s+2 && *(end-1) == '\n' && *(end-2) == '\n')
    --end;

  if (router_get_extrainfo_hash(s, digest) < 0) {
    log_warn(LD_DIR, "Couldn't compute router hash.");
    goto err;
  }
  tokens = smartlist_create();
  area = memarea_new();
  if (tokenize_string(area,s,end,tokens,extrainfo_token_table,0)) {
    log_warn(LD_DIR, "Error tokenizing extra-info document.");
    goto err;
  }

  if (smartlist_len(tokens) < 2) {
    log_warn(LD_DIR, "Impossibly short extra-info document.");
    goto err;
  }

  tok = smartlist_get(tokens,0);
  if (tok->tp != K_EXTRA_INFO) {
    log_warn(LD_DIR,"Entry does not start with \"extra-info\"");
    goto err;
  }

  extrainfo = tor_malloc_zero(sizeof(extrainfo_t));
  extrainfo->cache_info.is_extrainfo = 1;
  if (cache_copy)
    extrainfo->cache_info.signed_descriptor_body = tor_strndup(s, end-s);
  extrainfo->cache_info.signed_descriptor_len = end-s;
  memcpy(extrainfo->cache_info.signed_descriptor_digest, digest, DIGEST_LEN);

  tor_assert(tok->n_args >= 2);
  if (!is_legal_nickname(tok->args[0])) {
    log_warn(LD_DIR,"Bad nickname %s on \"extra-info\"",escaped(tok->args[0]));
    goto err;
  }
  strlcpy(extrainfo->nickname, tok->args[0], sizeof(extrainfo->nickname));
  if (strlen(tok->args[1]) != HEX_DIGEST_LEN ||
      base16_decode(extrainfo->cache_info.identity_digest, DIGEST_LEN,
                    tok->args[1], HEX_DIGEST_LEN)) {
    log_warn(LD_DIR,"Invalid fingerprint %s on \"extra-info\"",
             escaped(tok->args[1]));
    goto err;
  }

  tok = find_by_keyword(tokens, K_PUBLISHED);
  if (parse_iso_time(tok->args[0], &extrainfo->cache_info.published_on)) {
    log_warn(LD_DIR,"Invalid published time %s on \"extra-info\"",
             escaped(tok->args[0]));
    goto err;
  }

  if (routermap &&
      (router = digestmap_get((digestmap_t*)routermap,
                              extrainfo->cache_info.identity_digest))) {
    key = router->identity_pkey;
  }

  tok = find_by_keyword(tokens, K_ROUTER_SIGNATURE);
  if (strcmp(tok->object_type, "SIGNATURE") ||
      tok->object_size < 128 || tok->object_size > 512) {
    log_warn(LD_DIR, "Bad object type or length on extra-info signature");
    goto err;
  }

  if (key) {
    note_crypto_pk_op(VERIFY_RTR);
    if (check_signature_token(digest, DIGEST_LEN, tok, key, 0,
                              "extra-info") < 0)
      goto err;

    if (router)
      extrainfo->cache_info.send_unencrypted =
        router->cache_info.send_unencrypted;
  } else {
    extrainfo->pending_sig = tor_memdup(tok->object_body,
                                        tok->object_size);
    extrainfo->pending_sig_len = tok->object_size;
  }

  goto done;
 err:
  dump_desc(s_dup, "extra-info descriptor");
  extrainfo_free(extrainfo);
  extrainfo = NULL;
 done:
  if (tokens) {
    SMARTLIST_FOREACH(tokens, directory_token_t *, t, token_clear(t));
    smartlist_free(tokens);
  }
  if (area) {
    DUMP_AREA(area, "extrainfo");
    memarea_drop_all(area);
  }
  return extrainfo;
}