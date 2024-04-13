rend_parse_client_keys(strmap_t *parsed_clients, const char *ckstr)
{
  int result = -1;
  smartlist_t *tokens;
  directory_token_t *tok;
  const char *current_entry = NULL;
  memarea_t *area = NULL;
  if (!ckstr || strlen(ckstr) == 0)
    return -1;
  tokens = smartlist_create();
  /* Begin parsing with first entry, skipping comments or whitespace at the
   * beginning. */
  area = memarea_new();
  current_entry = eat_whitespace(ckstr);
  while (!strcmpstart(current_entry, "client-name ")) {
    rend_authorized_client_t *parsed_entry;
    size_t len;
    char descriptor_cookie_tmp[REND_DESC_COOKIE_LEN+2];
    /* Determine end of string. */
    const char *eos = strstr(current_entry, "\nclient-name ");
    if (!eos)
      eos = current_entry + strlen(current_entry);
    else
      eos = eos + 1;
    /* Free tokens and clear token list. */
    SMARTLIST_FOREACH(tokens, directory_token_t *, t, token_clear(t));
    smartlist_clear(tokens);
    memarea_clear(area);
    /* Tokenize string. */
    if (tokenize_string(area, current_entry, eos, tokens,
                        client_keys_token_table, 0)) {
      log_warn(LD_REND, "Error tokenizing client keys file.");
      goto err;
    }
    /* Advance to next entry, if available. */
    current_entry = eos;
    /* Check minimum allowed length of token list. */
    if (smartlist_len(tokens) < 2) {
      log_warn(LD_REND, "Impossibly short client key entry.");
      goto err;
    }
    /* Parse client name. */
    tok = find_by_keyword(tokens, C_CLIENT_NAME);
    tor_assert(tok == smartlist_get(tokens, 0));
    tor_assert(tok->n_args == 1);

    len = strlen(tok->args[0]);
    if (len < 1 || len > 19 ||
      strspn(tok->args[0], REND_LEGAL_CLIENTNAME_CHARACTERS) != len) {
      log_warn(LD_CONFIG, "Illegal client name: %s. (Length must be "
               "between 1 and 19, and valid characters are "
               "[A-Za-z0-9+-_].)", tok->args[0]);
      goto err;
    }
    /* Check if client name is duplicate. */
    if (strmap_get(parsed_clients, tok->args[0])) {
      log_warn(LD_CONFIG, "HiddenServiceAuthorizeClient contains a "
               "duplicate client name: '%s'. Ignoring.", tok->args[0]);
      goto err;
    }
    parsed_entry = tor_malloc_zero(sizeof(rend_authorized_client_t));
    parsed_entry->client_name = tor_strdup(tok->args[0]);
    strmap_set(parsed_clients, parsed_entry->client_name, parsed_entry);
    /* Parse client key. */
    tok = find_opt_by_keyword(tokens, C_CLIENT_KEY);
    if (tok) {
      parsed_entry->client_key = tok->key;
      tok->key = NULL; /* Prevent free */
    }

    /* Parse descriptor cookie. */
    tok = find_by_keyword(tokens, C_DESCRIPTOR_COOKIE);
    tor_assert(tok->n_args == 1);
    if (strlen(tok->args[0]) != REND_DESC_COOKIE_LEN_BASE64 + 2) {
      log_warn(LD_REND, "Descriptor cookie has illegal length: %s",
               escaped(tok->args[0]));
      goto err;
    }
    /* The size of descriptor_cookie_tmp needs to be REND_DESC_COOKIE_LEN+2,
     * because a base64 encoding of length 24 does not fit into 16 bytes in all
     * cases. */
    if ((base64_decode(descriptor_cookie_tmp, REND_DESC_COOKIE_LEN+2,
                       tok->args[0], REND_DESC_COOKIE_LEN_BASE64+2+1)
           != REND_DESC_COOKIE_LEN)) {
      log_warn(LD_REND, "Descriptor cookie contains illegal characters: "
               "%s", escaped(tok->args[0]));
      goto err;
    }
    memcpy(parsed_entry->descriptor_cookie, descriptor_cookie_tmp,
           REND_DESC_COOKIE_LEN);
  }
  result = strmap_size(parsed_clients);
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