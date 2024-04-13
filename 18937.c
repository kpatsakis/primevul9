routerstatus_parse_entry_from_string(memarea_t *area,
                                     const char **s, smartlist_t *tokens,
                                     networkstatus_t *vote,
                                     vote_routerstatus_t *vote_rs,
                                     int consensus_method,
                                     consensus_flavor_t flav)
{
  const char *eos, *s_dup = *s;
  routerstatus_t *rs = NULL;
  directory_token_t *tok;
  char timebuf[ISO_TIME_LEN+1];
  struct in_addr in;
  int offset = 0;
  tor_assert(tokens);
  tor_assert(bool_eq(vote, vote_rs));

  if (!consensus_method)
    flav = FLAV_NS;

  eos = find_start_of_next_routerstatus(*s);

  if (tokenize_string(area,*s, eos, tokens, rtrstatus_token_table,0)) {
    log_warn(LD_DIR, "Error tokenizing router status");
    goto err;
  }
  if (smartlist_len(tokens) < 1) {
    log_warn(LD_DIR, "Impossibly short router status");
    goto err;
  }
  tok = find_by_keyword(tokens, K_R);
  tor_assert(tok->n_args >= 7);
  if (flav == FLAV_NS) {
    if (tok->n_args < 8) {
      log_warn(LD_DIR, "Too few arguments to r");
      goto err;
    }
  } else {
    offset = -1;
  }
  if (vote_rs) {
    rs = &vote_rs->status;
  } else {
    rs = tor_malloc_zero(sizeof(routerstatus_t));
  }

  if (!is_legal_nickname(tok->args[0])) {
    log_warn(LD_DIR,
             "Invalid nickname %s in router status; skipping.",
             escaped(tok->args[0]));
    goto err;
  }
  strlcpy(rs->nickname, tok->args[0], sizeof(rs->nickname));

  if (digest_from_base64(rs->identity_digest, tok->args[1])) {
    log_warn(LD_DIR, "Error decoding identity digest %s",
             escaped(tok->args[1]));
    goto err;
  }

  if (flav == FLAV_NS) {
    if (digest_from_base64(rs->descriptor_digest, tok->args[2])) {
      log_warn(LD_DIR, "Error decoding descriptor digest %s",
               escaped(tok->args[2]));
      goto err;
    }
  }

  if (tor_snprintf(timebuf, sizeof(timebuf), "%s %s",
                   tok->args[3+offset], tok->args[4+offset]) < 0 ||
      parse_iso_time(timebuf, &rs->published_on)<0) {
    log_warn(LD_DIR, "Error parsing time '%s %s' [%d %d]",
             tok->args[3+offset], tok->args[4+offset],
             offset, (int)flav);
    goto err;
  }

  if (tor_inet_aton(tok->args[5+offset], &in) == 0) {
    log_warn(LD_DIR, "Error parsing router address in network-status %s",
             escaped(tok->args[5+offset]));
    goto err;
  }
  rs->addr = ntohl(in.s_addr);

  rs->or_port = (uint16_t) tor_parse_long(tok->args[6+offset],
                                         10,0,65535,NULL,NULL);
  rs->dir_port = (uint16_t) tor_parse_long(tok->args[7+offset],
                                           10,0,65535,NULL,NULL);

  tok = find_opt_by_keyword(tokens, K_S);
  if (tok && vote) {
    int i;
    vote_rs->flags = 0;
    for (i=0; i < tok->n_args; ++i) {
      int p = smartlist_string_pos(vote->known_flags, tok->args[i]);
      if (p >= 0) {
        vote_rs->flags |= (1<<p);
      } else {
        log_warn(LD_DIR, "Flags line had a flag %s not listed in known_flags.",
                 escaped(tok->args[i]));
        goto err;
      }
    }
  } else if (tok) {
    int i;
    for (i=0; i < tok->n_args; ++i) {
      if (!strcmp(tok->args[i], "Exit"))
        rs->is_exit = 1;
      else if (!strcmp(tok->args[i], "Stable"))
        rs->is_stable = 1;
      else if (!strcmp(tok->args[i], "Fast"))
        rs->is_fast = 1;
      else if (!strcmp(tok->args[i], "Running"))
        rs->is_running = 1;
      else if (!strcmp(tok->args[i], "Named"))
        rs->is_named = 1;
      else if (!strcmp(tok->args[i], "Valid"))
        rs->is_valid = 1;
      else if (!strcmp(tok->args[i], "V2Dir"))
        rs->is_v2_dir = 1;
      else if (!strcmp(tok->args[i], "Guard"))
        rs->is_possible_guard = 1;
      else if (!strcmp(tok->args[i], "BadExit"))
        rs->is_bad_exit = 1;
      else if (!strcmp(tok->args[i], "BadDirectory"))
        rs->is_bad_directory = 1;
      else if (!strcmp(tok->args[i], "Authority"))
        rs->is_authority = 1;
      else if (!strcmp(tok->args[i], "Unnamed") &&
               consensus_method >= 2) {
        /* Unnamed is computed right by consensus method 2 and later. */
        rs->is_unnamed = 1;
      } else if (!strcmp(tok->args[i], "HSDir")) {
        rs->is_hs_dir = 1;
      }
    }
  }
  if ((tok = find_opt_by_keyword(tokens, K_V))) {
    tor_assert(tok->n_args == 1);
    rs->version_known = 1;
    if (strcmpstart(tok->args[0], "Tor ")) {
      rs->version_supports_begindir = 1;
      rs->version_supports_extrainfo_upload = 1;
      rs->version_supports_conditional_consensus = 1;
    } else {
      rs->version_supports_begindir =
        tor_version_as_new_as(tok->args[0], "0.2.0.1-alpha");
      rs->version_supports_extrainfo_upload =
        tor_version_as_new_as(tok->args[0], "0.2.0.0-alpha-dev (r10070)");
      rs->version_supports_v3_dir =
        tor_version_as_new_as(tok->args[0], "0.2.0.8-alpha");
      rs->version_supports_conditional_consensus =
        tor_version_as_new_as(tok->args[0], "0.2.1.1-alpha");
    }
    if (vote_rs) {
      vote_rs->version = tor_strdup(tok->args[0]);
    }
  }

  /* handle weighting/bandwidth info */
  if ((tok = find_opt_by_keyword(tokens, K_W))) {
    int i;
    for (i=0; i < tok->n_args; ++i) {
      if (!strcmpstart(tok->args[i], "Bandwidth=")) {
        int ok;
        rs->bandwidth = (uint32_t)tor_parse_ulong(strchr(tok->args[i], '=')+1,
                                                  10, 0, UINT32_MAX,
                                                  &ok, NULL);
        if (!ok) {
          log_warn(LD_DIR, "Invalid Bandwidth %s", escaped(tok->args[i]));
          goto err;
        }
        rs->has_bandwidth = 1;
      } else if (!strcmpstart(tok->args[i], "Measured=")) {
        int ok;
        rs->measured_bw =
            (uint32_t)tor_parse_ulong(strchr(tok->args[i], '=')+1,
                                      10, 0, UINT32_MAX, &ok, NULL);
        if (!ok) {
          log_warn(LD_DIR, "Invalid Measured Bandwidth %s",
                   escaped(tok->args[i]));
          goto err;
        }
        rs->has_measured_bw = 1;
      }
    }
  }

  /* parse exit policy summaries */
  if ((tok = find_opt_by_keyword(tokens, K_P))) {
    tor_assert(tok->n_args == 1);
    if (strcmpstart(tok->args[0], "accept ") &&
        strcmpstart(tok->args[0], "reject ")) {
      log_warn(LD_DIR, "Unknown exit policy summary type %s.",
               escaped(tok->args[0]));
      goto err;
    }
    /* XXX weasel: parse this into ports and represent them somehow smart,
     * maybe not here but somewhere on if we need it for the client.
     * we should still parse it here to check it's valid tho.
     */
    rs->exitsummary = tor_strdup(tok->args[0]);
    rs->has_exitsummary = 1;
  }

  if (vote_rs) {
    SMARTLIST_FOREACH_BEGIN(tokens, directory_token_t *, t) {
      if (t->tp == K_M && t->n_args) {
        vote_microdesc_hash_t *line =
          tor_malloc(sizeof(vote_microdesc_hash_t));
        line->next = vote_rs->microdesc;
        line->microdesc_hash_line = tor_strdup(t->args[0]);
        vote_rs->microdesc = line;
      }
    } SMARTLIST_FOREACH_END(t);
  }

  if (!strcasecmp(rs->nickname, UNNAMED_ROUTER_NICKNAME))
    rs->is_named = 0;

  goto done;
 err:
  dump_desc(s_dup, "routerstatus entry");
  if (rs && !vote_rs)
    routerstatus_free(rs);
  rs = NULL;
 done:
  SMARTLIST_FOREACH(tokens, directory_token_t *, t, token_clear(t));
  smartlist_clear(tokens);
  if (area) {
    DUMP_AREA(area, "routerstatus entry");
    memarea_clear(area);
  }
  *s = eos;

  return rs;
}