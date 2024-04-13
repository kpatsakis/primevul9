rend_parse_v2_service_descriptor(rend_service_descriptor_t **parsed_out,
                                 char *desc_id_out,
                                 char **intro_points_encrypted_out,
                                 size_t *intro_points_encrypted_size_out,
                                 size_t *encoded_size_out,
                                 const char **next_out, const char *desc)
{
  rend_service_descriptor_t *result =
                            tor_malloc_zero(sizeof(rend_service_descriptor_t));
  char desc_hash[DIGEST_LEN];
  const char *eos;
  smartlist_t *tokens = smartlist_create();
  directory_token_t *tok;
  char secret_id_part[DIGEST_LEN];
  int i, version, num_ok=1;
  smartlist_t *versions;
  char public_key_hash[DIGEST_LEN];
  char test_desc_id[DIGEST_LEN];
  memarea_t *area = NULL;
  tor_assert(desc);
  /* Check if desc starts correctly. */
  if (strncmp(desc, "rendezvous-service-descriptor ",
              strlen("rendezvous-service-descriptor "))) {
    log_info(LD_REND, "Descriptor does not start correctly.");
    goto err;
  }
  /* Compute descriptor hash for later validation. */
  if (router_get_hash_impl(desc, strlen(desc), desc_hash,
                           "rendezvous-service-descriptor ",
                           "\nsignature", '\n', DIGEST_SHA1) < 0) {
    log_warn(LD_REND, "Couldn't compute descriptor hash.");
    goto err;
  }
  /* Determine end of string. */
  eos = strstr(desc, "\nrendezvous-service-descriptor ");
  if (!eos)
    eos = desc + strlen(desc);
  else
    eos = eos + 1;
  /* Check length. */
  if (eos-desc > REND_DESC_MAX_SIZE) {
    /* XXX023 If we are parsing this descriptor as a server, this
     * should be a protocol warning. */
    log_warn(LD_REND, "Descriptor length is %d which exceeds "
             "maximum rendezvous descriptor size of %d bytes.",
             (int)(eos-desc), REND_DESC_MAX_SIZE);
    goto err;
  }
  /* Tokenize descriptor. */
  area = memarea_new();
  if (tokenize_string(area, desc, eos, tokens, desc_token_table, 0)) {
    log_warn(LD_REND, "Error tokenizing descriptor.");
    goto err;
  }
  /* Set next to next descriptor, if available. */
  *next_out = eos;
  /* Set length of encoded descriptor. */
  *encoded_size_out = eos - desc;
  /* Check min allowed length of token list. */
  if (smartlist_len(tokens) < 7) {
    log_warn(LD_REND, "Impossibly short descriptor.");
    goto err;
  }
  /* Parse base32-encoded descriptor ID. */
  tok = find_by_keyword(tokens, R_RENDEZVOUS_SERVICE_DESCRIPTOR);
  tor_assert(tok == smartlist_get(tokens, 0));
  tor_assert(tok->n_args == 1);
  if (strlen(tok->args[0]) != REND_DESC_ID_V2_LEN_BASE32 ||
      strspn(tok->args[0], BASE32_CHARS) != REND_DESC_ID_V2_LEN_BASE32) {
    log_warn(LD_REND, "Invalid descriptor ID: '%s'", tok->args[0]);
    goto err;
  }
  if (base32_decode(desc_id_out, DIGEST_LEN,
                    tok->args[0], REND_DESC_ID_V2_LEN_BASE32) < 0) {
    log_warn(LD_REND, "Descriptor ID contains illegal characters: %s",
             tok->args[0]);
    goto err;
  }
  /* Parse descriptor version. */
  tok = find_by_keyword(tokens, R_VERSION);
  tor_assert(tok->n_args == 1);
  result->version =
    (int) tor_parse_long(tok->args[0], 10, 0, INT_MAX, &num_ok, NULL);
  if (result->version != 2 || !num_ok) {
    /* If it's <2, it shouldn't be under this format.  If the number
     * is greater than 2, we bumped it because we broke backward
     * compatibility.  See how version numbers in our other formats
     * work. */
    log_warn(LD_REND, "Unrecognized descriptor version: %s",
             escaped(tok->args[0]));
    goto err;
  }
  /* Parse public key. */
  tok = find_by_keyword(tokens, R_PERMANENT_KEY);
  result->pk = tok->key;
  tok->key = NULL; /* Prevent free */
  /* Parse secret ID part. */
  tok = find_by_keyword(tokens, R_SECRET_ID_PART);
  tor_assert(tok->n_args == 1);
  if (strlen(tok->args[0]) != REND_SECRET_ID_PART_LEN_BASE32 ||
      strspn(tok->args[0], BASE32_CHARS) != REND_SECRET_ID_PART_LEN_BASE32) {
    log_warn(LD_REND, "Invalid secret ID part: '%s'", tok->args[0]);
    goto err;
  }
  if (base32_decode(secret_id_part, DIGEST_LEN, tok->args[0], 32) < 0) {
    log_warn(LD_REND, "Secret ID part contains illegal characters: %s",
             tok->args[0]);
    goto err;
  }
  /* Parse publication time -- up-to-date check is done when storing the
   * descriptor. */
  tok = find_by_keyword(tokens, R_PUBLICATION_TIME);
  tor_assert(tok->n_args == 1);
  if (parse_iso_time(tok->args[0], &result->timestamp) < 0) {
    log_warn(LD_REND, "Invalid publication time: '%s'", tok->args[0]);
    goto err;
  }
  /* Parse protocol versions. */
  tok = find_by_keyword(tokens, R_PROTOCOL_VERSIONS);
  tor_assert(tok->n_args == 1);
  versions = smartlist_create();
  smartlist_split_string(versions, tok->args[0], ",",
                         SPLIT_SKIP_SPACE|SPLIT_IGNORE_BLANK, 0);
  for (i = 0; i < smartlist_len(versions); i++) {
    version = (int) tor_parse_long(smartlist_get(versions, i),
                                   10, 0, INT_MAX, &num_ok, NULL);
    if (!num_ok) /* It's a string; let's ignore it. */
      continue;
    result->protocols |= 1 << version;
  }
  SMARTLIST_FOREACH(versions, char *, cp, tor_free(cp));
  smartlist_free(versions);
  /* Parse encrypted introduction points. Don't verify. */
  tok = find_opt_by_keyword(tokens, R_INTRODUCTION_POINTS);
  if (tok) {
    if (strcmp(tok->object_type, "MESSAGE")) {
      log_warn(LD_DIR, "Bad object type: introduction points should be of "
               "type MESSAGE");
      goto err;
    }
    *intro_points_encrypted_out = tor_memdup(tok->object_body,
                                             tok->object_size);
    *intro_points_encrypted_size_out = tok->object_size;
  } else {
    *intro_points_encrypted_out = NULL;
    *intro_points_encrypted_size_out = 0;
  }
  /* Parse and verify signature. */
  tok = find_by_keyword(tokens, R_SIGNATURE);
  note_crypto_pk_op(VERIFY_RTR);
  if (check_signature_token(desc_hash, DIGEST_LEN, tok, result->pk, 0,
                            "v2 rendezvous service descriptor") < 0)
    goto err;
  /* Verify that descriptor ID belongs to public key and secret ID part. */
  crypto_pk_get_digest(result->pk, public_key_hash);
  rend_get_descriptor_id_bytes(test_desc_id, public_key_hash,
                               secret_id_part);
  if (tor_memneq(desc_id_out, test_desc_id, DIGEST_LEN)) {
    log_warn(LD_REND, "Parsed descriptor ID does not match "
             "computed descriptor ID.");
    goto err;
  }
  goto done;
 err:
  rend_service_descriptor_free(result);
  result = NULL;
 done:
  if (tokens) {
    SMARTLIST_FOREACH(tokens, directory_token_t *, t, token_clear(t));
    smartlist_free(tokens);
  }
  if (area)
    memarea_drop_all(area);
  *parsed_out = result;
  if (result)
    return 0;
  return -1;
}