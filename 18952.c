router_parse_entry_from_string(const char *s, const char *end,
                               int cache_copy, int allow_annotations,
                               const char *prepend_annotations)
{
  routerinfo_t *router = NULL;
  char digest[128];
  smartlist_t *tokens = NULL, *exit_policy_tokens = NULL;
  directory_token_t *tok;
  struct in_addr in;
  const char *start_of_annotations, *cp, *s_dup = s;
  size_t prepend_len = prepend_annotations ? strlen(prepend_annotations) : 0;
  int ok = 1;
  memarea_t *area = NULL;

  tor_assert(!allow_annotations || !prepend_annotations);

  if (!end) {
    end = s + strlen(s);
  }

  /* point 'end' to a point immediately after the final newline. */
  while (end > s+2 && *(end-1) == '\n' && *(end-2) == '\n')
    --end;

  area = memarea_new();
  tokens = smartlist_create();
  if (prepend_annotations) {
    if (tokenize_string(area,prepend_annotations,NULL,tokens,
                        routerdesc_token_table,TS_NOCHECK)) {
      log_warn(LD_DIR, "Error tokenizing router descriptor (annotations).");
      goto err;
    }
  }

  start_of_annotations = s;
  cp = tor_memstr(s, end-s, "\nrouter ");
  if (!cp) {
    if (end-s < 7 || strcmpstart(s, "router ")) {
      log_warn(LD_DIR, "No router keyword found.");
      goto err;
    }
  } else {
    s = cp+1;
  }

  if (start_of_annotations != s) { /* We have annotations */
    if (allow_annotations) {
      if (tokenize_string(area,start_of_annotations,s,tokens,
                          routerdesc_token_table,TS_NOCHECK)) {
        log_warn(LD_DIR, "Error tokenizing router descriptor (annotations).");
        goto err;
      }
    } else {
      log_warn(LD_DIR, "Found unexpected annotations on router descriptor not "
               "loaded from disk.  Dropping it.");
      goto err;
    }
  }

  if (router_get_router_hash(s, end - s, digest) < 0) {
    log_warn(LD_DIR, "Couldn't compute router hash.");
    goto err;
  }
  {
    int flags = 0;
    if (allow_annotations)
      flags |= TS_ANNOTATIONS_OK;
    if (prepend_annotations)
      flags |= TS_ANNOTATIONS_OK|TS_NO_NEW_ANNOTATIONS;

    if (tokenize_string(area,s,end,tokens,routerdesc_token_table, flags)) {
      log_warn(LD_DIR, "Error tokenizing router descriptor.");
      goto err;
    }
  }

  if (smartlist_len(tokens) < 2) {
    log_warn(LD_DIR, "Impossibly short router descriptor.");
    goto err;
  }

  tok = find_by_keyword(tokens, K_ROUTER);
  tor_assert(tok->n_args >= 5);

  router = tor_malloc_zero(sizeof(routerinfo_t));
  router->country = -1;
  router->cache_info.routerlist_index = -1;
  router->cache_info.annotations_len = s-start_of_annotations + prepend_len;
  router->cache_info.signed_descriptor_len = end-s;
  if (cache_copy) {
    size_t len = router->cache_info.signed_descriptor_len +
                 router->cache_info.annotations_len;
    char *cp =
      router->cache_info.signed_descriptor_body = tor_malloc(len+1);
    if (prepend_annotations) {
      memcpy(cp, prepend_annotations, prepend_len);
      cp += prepend_len;
    }
    /* This assertion will always succeed.
     * len == signed_desc_len + annotations_len
     *     == end-s + s-start_of_annotations + prepend_len
     *     == end-start_of_annotations + prepend_len
     * We already wrote prepend_len bytes into the buffer; now we're
     * writing end-start_of_annotations -NM. */
    tor_assert(cp+(end-start_of_annotations) ==
               router->cache_info.signed_descriptor_body+len);
    memcpy(cp, start_of_annotations, end-start_of_annotations);
    router->cache_info.signed_descriptor_body[len] = '\0';
    tor_assert(strlen(router->cache_info.signed_descriptor_body) == len);
  }
  memcpy(router->cache_info.signed_descriptor_digest, digest, DIGEST_LEN);

  router->nickname = tor_strdup(tok->args[0]);
  if (!is_legal_nickname(router->nickname)) {
    log_warn(LD_DIR,"Router nickname is invalid");
    goto err;
  }
  router->address = tor_strdup(tok->args[1]);
  if (!tor_inet_aton(router->address, &in)) {
    log_warn(LD_DIR,"Router address is not an IP address.");
    goto err;
  }
  router->addr = ntohl(in.s_addr);

  router->or_port =
    (uint16_t) tor_parse_long(tok->args[2],10,0,65535,&ok,NULL);
  if (!ok) {
    log_warn(LD_DIR,"Invalid OR port %s", escaped(tok->args[2]));
    goto err;
  }
  router->dir_port =
    (uint16_t) tor_parse_long(tok->args[4],10,0,65535,&ok,NULL);
  if (!ok) {
    log_warn(LD_DIR,"Invalid dir port %s", escaped(tok->args[4]));
    goto err;
  }

  tok = find_by_keyword(tokens, K_BANDWIDTH);
  tor_assert(tok->n_args >= 3);
  router->bandwidthrate = (int)
    tor_parse_long(tok->args[0],10,1,INT_MAX,&ok,NULL);

  if (!ok) {
    log_warn(LD_DIR, "bandwidthrate %s unreadable or 0. Failing.",
             escaped(tok->args[0]));
    goto err;
  }
  router->bandwidthburst =
    (int) tor_parse_long(tok->args[1],10,0,INT_MAX,&ok,NULL);
  if (!ok) {
    log_warn(LD_DIR, "Invalid bandwidthburst %s", escaped(tok->args[1]));
    goto err;
  }
  router->bandwidthcapacity = (int)
    tor_parse_long(tok->args[2],10,0,INT_MAX,&ok,NULL);
  if (!ok) {
    log_warn(LD_DIR, "Invalid bandwidthcapacity %s", escaped(tok->args[1]));
    goto err;
  }

  if ((tok = find_opt_by_keyword(tokens, A_PURPOSE))) {
    tor_assert(tok->n_args);
    router->purpose = router_purpose_from_string(tok->args[0]);
  } else {
    router->purpose = ROUTER_PURPOSE_GENERAL;
  }
  router->cache_info.send_unencrypted =
    (router->purpose == ROUTER_PURPOSE_GENERAL) ? 1 : 0;

  if ((tok = find_opt_by_keyword(tokens, K_UPTIME))) {
    tor_assert(tok->n_args >= 1);
    router->uptime = tor_parse_long(tok->args[0],10,0,LONG_MAX,&ok,NULL);
    if (!ok) {
      log_warn(LD_DIR, "Invalid uptime %s", escaped(tok->args[0]));
      goto err;
    }
  }

  if ((tok = find_opt_by_keyword(tokens, K_HIBERNATING))) {
    tor_assert(tok->n_args >= 1);
    router->is_hibernating
      = (tor_parse_long(tok->args[0],10,0,LONG_MAX,NULL,NULL) != 0);
  }

  tok = find_by_keyword(tokens, K_PUBLISHED);
  tor_assert(tok->n_args == 1);
  if (parse_iso_time(tok->args[0], &router->cache_info.published_on) < 0)
    goto err;

  tok = find_by_keyword(tokens, K_ONION_KEY);
  if (!crypto_pk_public_exponent_ok(tok->key)) {
    log_warn(LD_DIR,
             "Relay's onion key had invalid exponent.");
    goto err;
  }
  router->onion_pkey = tok->key;
  tok->key = NULL; /* Prevent free */

  tok = find_by_keyword(tokens, K_SIGNING_KEY);
  router->identity_pkey = tok->key;
  tok->key = NULL; /* Prevent free */
  if (crypto_pk_get_digest(router->identity_pkey,
                           router->cache_info.identity_digest)) {
    log_warn(LD_DIR, "Couldn't calculate key digest"); goto err;
  }

  if ((tok = find_opt_by_keyword(tokens, K_FINGERPRINT))) {
    /* If there's a fingerprint line, it must match the identity digest. */
    char d[DIGEST_LEN];
    tor_assert(tok->n_args == 1);
    tor_strstrip(tok->args[0], " ");
    if (base16_decode(d, DIGEST_LEN, tok->args[0], strlen(tok->args[0]))) {
      log_warn(LD_DIR, "Couldn't decode router fingerprint %s",
               escaped(tok->args[0]));
      goto err;
    }
    if (tor_memneq(d,router->cache_info.identity_digest, DIGEST_LEN)) {
      log_warn(LD_DIR, "Fingerprint '%s' does not match identity digest.",
               tok->args[0]);
      goto err;
    }
  }

  if ((tok = find_opt_by_keyword(tokens, K_PLATFORM))) {
    router->platform = tor_strdup(tok->args[0]);
  }

  if ((tok = find_opt_by_keyword(tokens, K_CONTACT))) {
    router->contact_info = tor_strdup(tok->args[0]);
  }

  if ((tok = find_opt_by_keyword(tokens, K_EVENTDNS))) {
    router->has_old_dnsworkers = tok->n_args && !strcmp(tok->args[0], "0");
  } else if (router->platform) {
    if (! tor_version_as_new_as(router->platform, "0.1.2.2-alpha"))
      router->has_old_dnsworkers = 1;
  }

  if (find_opt_by_keyword(tokens, K_REJECT6) ||
      find_opt_by_keyword(tokens, K_ACCEPT6)) {
    log_warn(LD_DIR, "Rejecting router with reject6/accept6 line: they crash "
             "older Tors.");
    goto err;
  }
  exit_policy_tokens = find_all_exitpolicy(tokens);
  if (!smartlist_len(exit_policy_tokens)) {
    log_warn(LD_DIR, "No exit policy tokens in descriptor.");
    goto err;
  }
  SMARTLIST_FOREACH(exit_policy_tokens, directory_token_t *, t,
                    if (router_add_exit_policy(router,t)<0) {
                      log_warn(LD_DIR,"Error in exit policy");
                      goto err;
                    });
  policy_expand_private(&router->exit_policy);
  if (policy_is_reject_star(router->exit_policy))
    router->policy_is_reject_star = 1;

  if ((tok = find_opt_by_keyword(tokens, K_FAMILY)) && tok->n_args) {
    int i;
    router->declared_family = smartlist_create();
    for (i=0;i<tok->n_args;++i) {
      if (!is_legal_nickname_or_hexdigest(tok->args[i])) {
        log_warn(LD_DIR, "Illegal nickname %s in family line",
                 escaped(tok->args[i]));
        goto err;
      }
      smartlist_add(router->declared_family, tor_strdup(tok->args[i]));
    }
  }

  if (find_opt_by_keyword(tokens, K_CACHES_EXTRA_INFO))
    router->caches_extra_info = 1;

  if (find_opt_by_keyword(tokens, K_ALLOW_SINGLE_HOP_EXITS))
    router->allow_single_hop_exits = 1;

  if ((tok = find_opt_by_keyword(tokens, K_EXTRA_INFO_DIGEST))) {
    tor_assert(tok->n_args >= 1);
    if (strlen(tok->args[0]) == HEX_DIGEST_LEN) {
      base16_decode(router->cache_info.extra_info_digest,
                    DIGEST_LEN, tok->args[0], HEX_DIGEST_LEN);
    } else {
      log_warn(LD_DIR, "Invalid extra info digest %s", escaped(tok->args[0]));
    }
  }

  if (find_opt_by_keyword(tokens, K_HIDDEN_SERVICE_DIR)) {
    router->wants_to_be_hs_dir = 1;
  }

  tok = find_by_keyword(tokens, K_ROUTER_SIGNATURE);
  note_crypto_pk_op(VERIFY_RTR);
#ifdef COUNT_DISTINCT_DIGESTS
  if (!verified_digests)
    verified_digests = digestmap_new();
  digestmap_set(verified_digests, signed_digest, (void*)(uintptr_t)1);
#endif
  if (check_signature_token(digest, DIGEST_LEN, tok, router->identity_pkey, 0,
                            "router descriptor") < 0)
    goto err;

  routerinfo_set_country(router);

  if (!router->or_port) {
    log_warn(LD_DIR,"or_port unreadable or 0. Failing.");
    goto err;
  }

  if (!router->platform) {
    router->platform = tor_strdup("<unknown>");
  }

  goto done;

 err:
  dump_desc(s_dup, "router descriptor");
  routerinfo_free(router);
  router = NULL;
 done:
  if (tokens) {
    SMARTLIST_FOREACH(tokens, directory_token_t *, t, token_clear(t));
    smartlist_free(tokens);
  }
  smartlist_free(exit_policy_tokens);
  if (area) {
    DUMP_AREA(area, "routerinfo");
    memarea_drop_all(area);
  }
  return router;
}