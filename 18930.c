rend_parse_introduction_points(rend_service_descriptor_t *parsed,
                               const char *intro_points_encoded,
                               size_t intro_points_encoded_size)
{
  const char *current_ipo, *end_of_intro_points;
  smartlist_t *tokens;
  directory_token_t *tok;
  rend_intro_point_t *intro;
  extend_info_t *info;
  int result, num_ok=1;
  memarea_t *area = NULL;
  tor_assert(parsed);
  /** Function may only be invoked once. */
  tor_assert(!parsed->intro_nodes);
  tor_assert(intro_points_encoded);
  tor_assert(intro_points_encoded_size > 0);
  /* Consider one intro point after the other. */
  current_ipo = intro_points_encoded;
  end_of_intro_points = intro_points_encoded + intro_points_encoded_size;
  tokens = smartlist_create();
  parsed->intro_nodes = smartlist_create();
  area = memarea_new();

  while (!fast_memcmpstart(current_ipo, end_of_intro_points-current_ipo,
                      "introduction-point ")) {
    /* Determine end of string. */
    const char *eos = tor_memstr(current_ipo, end_of_intro_points-current_ipo,
                                 "\nintroduction-point ");
    if (!eos)
      eos = end_of_intro_points;
    else
      eos = eos+1;
    tor_assert(eos <= intro_points_encoded+intro_points_encoded_size);
    /* Free tokens and clear token list. */
    SMARTLIST_FOREACH(tokens, directory_token_t *, t, token_clear(t));
    smartlist_clear(tokens);
    memarea_clear(area);
    /* Tokenize string. */
    if (tokenize_string(area, current_ipo, eos, tokens, ipo_token_table, 0)) {
      log_warn(LD_REND, "Error tokenizing introduction point");
      goto err;
    }
    /* Advance to next introduction point, if available. */
    current_ipo = eos;
    /* Check minimum allowed length of introduction point. */
    if (smartlist_len(tokens) < 5) {
      log_warn(LD_REND, "Impossibly short introduction point.");
      goto err;
    }
    /* Allocate new intro point and extend info. */
    intro = tor_malloc_zero(sizeof(rend_intro_point_t));
    info = intro->extend_info = tor_malloc_zero(sizeof(extend_info_t));
    /* Parse identifier. */
    tok = find_by_keyword(tokens, R_IPO_IDENTIFIER);
    if (base32_decode(info->identity_digest, DIGEST_LEN,
                      tok->args[0], REND_INTRO_POINT_ID_LEN_BASE32) < 0) {
      log_warn(LD_REND, "Identity digest contains illegal characters: %s",
               tok->args[0]);
      rend_intro_point_free(intro);
      goto err;
    }
    /* Write identifier to nickname. */
    info->nickname[0] = '$';
    base16_encode(info->nickname + 1, sizeof(info->nickname) - 1,
                  info->identity_digest, DIGEST_LEN);
    /* Parse IP address. */
    tok = find_by_keyword(tokens, R_IPO_IP_ADDRESS);
    if (tor_addr_from_str(&info->addr, tok->args[0])<0) {
      log_warn(LD_REND, "Could not parse introduction point address.");
      rend_intro_point_free(intro);
      goto err;
    }
    if (tor_addr_family(&info->addr) != AF_INET) {
      log_warn(LD_REND, "Introduction point address was not ipv4.");
      rend_intro_point_free(intro);
      goto err;
    }

    /* Parse onion port. */
    tok = find_by_keyword(tokens, R_IPO_ONION_PORT);
    info->port = (uint16_t) tor_parse_long(tok->args[0],10,1,65535,
                                           &num_ok,NULL);
    if (!info->port || !num_ok) {
      log_warn(LD_REND, "Introduction point onion port %s is invalid",
               escaped(tok->args[0]));
      rend_intro_point_free(intro);
      goto err;
    }
    /* Parse onion key. */
    tok = find_by_keyword(tokens, R_IPO_ONION_KEY);
    if (!crypto_pk_public_exponent_ok(tok->key)) {
      log_warn(LD_REND,
               "Introduction point's onion key had invalid exponent.");
      rend_intro_point_free(intro);
      goto err;
    }
    info->onion_key = tok->key;
    tok->key = NULL; /* Prevent free */
    /* Parse service key. */
    tok = find_by_keyword(tokens, R_IPO_SERVICE_KEY);
    if (!crypto_pk_public_exponent_ok(tok->key)) {
      log_warn(LD_REND,
               "Introduction point key had invalid exponent.");
      rend_intro_point_free(intro);
      goto err;
    }
    intro->intro_key = tok->key;
    tok->key = NULL; /* Prevent free */
    /* Add extend info to list of introduction points. */
    smartlist_add(parsed->intro_nodes, intro);
  }
  result = smartlist_len(parsed->intro_nodes);
  goto done;

 err:
  result = -1;

 done:
  /* Free tokens and clear token list. */
  SMARTLIST_FOREACH(tokens, directory_token_t *, t, token_clear(t));
  smartlist_free(tokens);
  if (area)
    memarea_drop_all(area);

  return result;
}