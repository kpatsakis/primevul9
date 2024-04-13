dirserv_generate_networkstatus_vote_obj(crypto_pk_t *private_key,
                                        authority_cert_t *cert)
{
  const or_options_t *options = get_options();
  networkstatus_t *v3_out = NULL;
  uint32_t addr;
  char *hostname = NULL, *client_versions = NULL, *server_versions = NULL;
  const char *contact;
  smartlist_t *routers, *routerstatuses;
  char identity_digest[DIGEST_LEN];
  char signing_key_digest[DIGEST_LEN];
  int listbadexits = options->AuthDirListBadExits;
  routerlist_t *rl = router_get_routerlist();
  time_t now = time(NULL);
  time_t cutoff = now - ROUTER_MAX_AGE_TO_PUBLISH;
  networkstatus_voter_info_t *voter = NULL;
  vote_timing_t timing;
  digestmap_t *omit_as_sybil = NULL;
  const int vote_on_reachability = running_long_enough_to_decide_unreachable();
  smartlist_t *microdescriptors = NULL;

  tor_assert(private_key);
  tor_assert(cert);

  if (crypto_pk_get_digest(private_key, signing_key_digest)<0) {
    log_err(LD_BUG, "Error computing signing key digest");
    return NULL;
  }
  if (crypto_pk_get_digest(cert->identity_key, identity_digest)<0) {
    log_err(LD_BUG, "Error computing identity key digest");
    return NULL;
  }
  if (resolve_my_address(LOG_WARN, options, &addr, NULL, &hostname)<0) {
    log_warn(LD_NET, "Couldn't resolve my hostname");
    return NULL;
  }
  if (!hostname || !strchr(hostname, '.')) {
    tor_free(hostname);
    hostname = tor_dup_ip(addr);
  }

  if (options->VersioningAuthoritativeDir) {
    client_versions = format_versions_list(options->RecommendedClientVersions);
    server_versions = format_versions_list(options->RecommendedServerVersions);
  }

  contact = get_options()->ContactInfo;
  if (!contact)
    contact = "(none)";

  /*
   * Do this so dirserv_compute_performance_thresholds() and
   * set_routerstatus_from_routerinfo() see up-to-date bandwidth info.
   */
  if (options->V3BandwidthsFile) {
    dirserv_read_measured_bandwidths(options->V3BandwidthsFile, NULL);
  } else {
    /*
     * No bandwidths file; clear the measured bandwidth cache in case we had
     * one last time around.
     */
    if (dirserv_get_measured_bw_cache_size() > 0) {
      dirserv_clear_measured_bw_cache();
    }
  }

  /* precompute this part, since we need it to decide what "stable"
   * means. */
  SMARTLIST_FOREACH(rl->routers, routerinfo_t *, ri, {
    dirserv_set_router_is_running(ri, now);
  });

  routers = smartlist_new();
  smartlist_add_all(routers, rl->routers);
  routers_make_ed_keys_unique(routers);
  /* After this point, don't use rl->routers; use 'routers' instead. */
  routers_sort_by_identity(routers);
  omit_as_sybil = get_possible_sybil_list(routers);

  DIGESTMAP_FOREACH(omit_as_sybil, sybil_id, void *, ignore) {
    (void) ignore;
    rep_hist_make_router_pessimal(sybil_id, now);
  } DIGESTMAP_FOREACH_END;

  /* Count how many have measured bandwidths so we know how to assign flags;
   * this must come before dirserv_compute_performance_thresholds() */
  dirserv_count_measured_bws(routers);

  dirserv_compute_performance_thresholds(omit_as_sybil);

  routerstatuses = smartlist_new();
  microdescriptors = smartlist_new();

  SMARTLIST_FOREACH_BEGIN(routers, routerinfo_t *, ri) {
    if (ri->cache_info.published_on >= cutoff) {
      routerstatus_t *rs;
      vote_routerstatus_t *vrs;
      node_t *node = node_get_mutable_by_id(ri->cache_info.identity_digest);
      if (!node)
        continue;

      vrs = tor_malloc_zero(sizeof(vote_routerstatus_t));
      rs = &vrs->status;
      set_routerstatus_from_routerinfo(rs, node, ri, now,
                                       listbadexits);

      if (ri->cache_info.signing_key_cert) {
        memcpy(vrs->ed25519_id,
               ri->cache_info.signing_key_cert->signing_key.pubkey,
               ED25519_PUBKEY_LEN);
      }

      if (digestmap_get(omit_as_sybil, ri->cache_info.identity_digest))
        clear_status_flags_on_sybil(rs);

      if (!vote_on_reachability)
        rs->is_flagged_running = 0;

      vrs->version = version_from_platform(ri->platform);
      if (ri->protocol_list) {
        vrs->protocols = tor_strdup(ri->protocol_list);
      } else {
        vrs->protocols = tor_strdup(
                              protover_compute_for_old_tor(vrs->version));
      }
      vrs->microdesc = dirvote_format_all_microdesc_vote_lines(ri, now,
                                                        microdescriptors);

      smartlist_add(routerstatuses, vrs);
    }
  } SMARTLIST_FOREACH_END(ri);

  {
    smartlist_t *added =
      microdescs_add_list_to_cache(get_microdesc_cache(),
                                   microdescriptors, SAVED_NOWHERE, 0);
    smartlist_free(added);
    smartlist_free(microdescriptors);
  }

  smartlist_free(routers);
  digestmap_free(omit_as_sybil, NULL);

  /* Apply guardfraction information to routerstatuses. */
  if (options->GuardfractionFile) {
    dirserv_read_guardfraction_file(options->GuardfractionFile,
                                    routerstatuses);
  }

  /* This pass through applies the measured bw lines to the routerstatuses */
  if (options->V3BandwidthsFile) {
    dirserv_read_measured_bandwidths(options->V3BandwidthsFile,
                                     routerstatuses);
  } else {
    /*
     * No bandwidths file; clear the measured bandwidth cache in case we had
     * one last time around.
     */
    if (dirserv_get_measured_bw_cache_size() > 0) {
      dirserv_clear_measured_bw_cache();
    }
  }

  v3_out = tor_malloc_zero(sizeof(networkstatus_t));

  v3_out->type = NS_TYPE_VOTE;
  dirvote_get_preferred_voting_intervals(&timing);
  v3_out->published = now;
  {
    char tbuf[ISO_TIME_LEN+1];
    networkstatus_t *current_consensus =
      networkstatus_get_live_consensus(now);
    long last_consensus_interval; /* only used to pick a valid_after */
    if (current_consensus)
      last_consensus_interval = current_consensus->fresh_until -
        current_consensus->valid_after;
    else
      last_consensus_interval = options->TestingV3AuthInitialVotingInterval;
    v3_out->valid_after =
      dirvote_get_start_of_next_interval(now, (int)last_consensus_interval,
                                      options->TestingV3AuthVotingStartOffset);
    format_iso_time(tbuf, v3_out->valid_after);
    log_notice(LD_DIR,"Choosing valid-after time in vote as %s: "
               "consensus_set=%d, last_interval=%d",
               tbuf, current_consensus?1:0, (int)last_consensus_interval);
  }
  v3_out->fresh_until = v3_out->valid_after + timing.vote_interval;
  v3_out->valid_until = v3_out->valid_after +
    (timing.vote_interval * timing.n_intervals_valid);
  v3_out->vote_seconds = timing.vote_delay;
  v3_out->dist_seconds = timing.dist_delay;
  tor_assert(v3_out->vote_seconds > 0);
  tor_assert(v3_out->dist_seconds > 0);
  tor_assert(timing.n_intervals_valid > 0);

  v3_out->client_versions = client_versions;
  v3_out->server_versions = server_versions;

  /* These are hardwired, to avoid disaster. */
  v3_out->recommended_relay_protocols =
    tor_strdup("Cons=1-2 Desc=1-2 DirCache=1 HSDir=1 HSIntro=3 HSRend=1 "
               "Link=4 LinkAuth=1 Microdesc=1-2 Relay=2");
  v3_out->recommended_client_protocols =
    tor_strdup("Cons=1-2 Desc=1-2 DirCache=1 HSDir=1 HSIntro=3 HSRend=1 "
               "Link=4 LinkAuth=1 Microdesc=1-2 Relay=2");
  v3_out->required_client_protocols =
    tor_strdup("Cons=1-2 Desc=1-2 DirCache=1 HSDir=1 HSIntro=3 HSRend=1 "
               "Link=4 LinkAuth=1 Microdesc=1-2 Relay=2");
  v3_out->required_relay_protocols =
    tor_strdup("Cons=1 Desc=1 DirCache=1 HSDir=1 HSIntro=3 HSRend=1 "
               "Link=3-4 LinkAuth=1 Microdesc=1 Relay=1-2");

  /* We are not allowed to vote to require anything we don't have. */
  tor_assert(protover_all_supported(v3_out->required_relay_protocols, NULL));
  tor_assert(protover_all_supported(v3_out->required_client_protocols, NULL));

  /* We should not recommend anything we don't have. */
  tor_assert_nonfatal(protover_all_supported(
                         v3_out->recommended_relay_protocols, NULL));
  tor_assert_nonfatal(protover_all_supported(
                         v3_out->recommended_client_protocols, NULL));

  v3_out->package_lines = smartlist_new();
  {
    config_line_t *cl;
    for (cl = get_options()->RecommendedPackages; cl; cl = cl->next) {
      if (validate_recommended_package_line(cl->value))
        smartlist_add(v3_out->package_lines, tor_strdup(cl->value));
    }
  }

  v3_out->known_flags = smartlist_new();
  smartlist_split_string(v3_out->known_flags,
                "Authority Exit Fast Guard Stable V2Dir Valid HSDir",
                0, SPLIT_SKIP_SPACE|SPLIT_IGNORE_BLANK, 0);
  if (vote_on_reachability)
    smartlist_add(v3_out->known_flags, tor_strdup("Running"));
  if (listbadexits)
    smartlist_add(v3_out->known_flags, tor_strdup("BadExit"));
  smartlist_sort_strings(v3_out->known_flags);

  if (options->ConsensusParams) {
    v3_out->net_params = smartlist_new();
    smartlist_split_string(v3_out->net_params,
                           options->ConsensusParams, NULL, 0, 0);
    smartlist_sort_strings(v3_out->net_params);
  }

  voter = tor_malloc_zero(sizeof(networkstatus_voter_info_t));
  voter->nickname = tor_strdup(options->Nickname);
  memcpy(voter->identity_digest, identity_digest, DIGEST_LEN);
  voter->sigs = smartlist_new();
  voter->address = hostname;
  voter->addr = addr;
  voter->dir_port = router_get_advertised_dir_port(options, 0);
  voter->or_port = router_get_advertised_or_port(options);
  voter->contact = tor_strdup(contact);
  if (options->V3AuthUseLegacyKey) {
    authority_cert_t *c = get_my_v3_legacy_cert();
    if (c) {
      if (crypto_pk_get_digest(c->identity_key, voter->legacy_id_digest)) {
        log_warn(LD_BUG, "Unable to compute digest of legacy v3 identity key");
        memset(voter->legacy_id_digest, 0, DIGEST_LEN);
      }
    }
  }

  v3_out->voters = smartlist_new();
  smartlist_add(v3_out->voters, voter);
  v3_out->cert = authority_cert_dup(cert);
  v3_out->routerstatus_list = routerstatuses;
  /* Note: networkstatus_digest is unset; it won't get set until we actually
   * format the vote. */

  return v3_out;
}