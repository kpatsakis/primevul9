microdescs_parse_from_string(const char *s, const char *eos,
                             int allow_annotations, int copy_body)
{
  smartlist_t *tokens;
  smartlist_t *result;
  microdesc_t *md = NULL;
  memarea_t *area;
  const char *start = s;
  const char *start_of_next_microdesc;
  int flags = allow_annotations ? TS_ANNOTATIONS_OK : 0;

  directory_token_t *tok;

  if (!eos)
    eos = s + strlen(s);

  s = eat_whitespace_eos(s, eos);
  area = memarea_new();
  result = smartlist_create();
  tokens = smartlist_create();

  while (s < eos) {
    start_of_next_microdesc = find_start_of_next_microdesc(s, eos);
    if (!start_of_next_microdesc)
      start_of_next_microdesc = eos;

    if (tokenize_string(area, s, start_of_next_microdesc, tokens,
                        microdesc_token_table, flags)) {
      log_warn(LD_DIR, "Unparseable microdescriptor");
      goto next;
    }

    md = tor_malloc_zero(sizeof(microdesc_t));
    {
      const char *cp = tor_memstr(s, start_of_next_microdesc-s,
                                  "onion-key");
      tor_assert(cp);

      md->bodylen = start_of_next_microdesc - cp;
      if (copy_body)
        md->body = tor_strndup(cp, md->bodylen);
      else
        md->body = (char*)cp;
      md->off = cp - start;
    }

    if ((tok = find_opt_by_keyword(tokens, A_LAST_LISTED))) {
      if (parse_iso_time(tok->args[0], &md->last_listed)) {
        log_warn(LD_DIR, "Bad last-listed time in microdescriptor");
        goto next;
      }
    }

    tok = find_by_keyword(tokens, K_ONION_KEY);
    if (!crypto_pk_public_exponent_ok(tok->key)) {
      log_warn(LD_DIR,
               "Relay's onion key had invalid exponent.");
      goto next;
    }
    md->onion_pkey = tok->key;
    tok->key = NULL;

    if ((tok = find_opt_by_keyword(tokens, K_FAMILY))) {
      int i;
      md->family = smartlist_create();
      for (i=0;i<tok->n_args;++i) {
        if (!is_legal_nickname_or_hexdigest(tok->args[i])) {
          log_warn(LD_DIR, "Illegal nickname %s in family line",
                   escaped(tok->args[i]));
          goto next;
        }
        smartlist_add(md->family, tor_strdup(tok->args[i]));
      }
    }

    if ((tok = find_opt_by_keyword(tokens, K_P))) {
      md->exitsummary = tor_strdup(tok->args[0]);
    }

    crypto_digest256(md->digest, md->body, md->bodylen, DIGEST_SHA256);

    smartlist_add(result, md);

    md = NULL;
  next:
    microdesc_free(md);
    md = NULL;

    memarea_clear(area);
    smartlist_clear(tokens);
    s = start_of_next_microdesc;
  }

  memarea_drop_all(area);
  smartlist_free(tokens);

  return result;
}