routerstatus_format_entry(const routerstatus_t *rs, const char *version,
                          const char *protocols,
                          routerstatus_format_type_t format,
                          const vote_routerstatus_t *vrs)
{
  char *summary;
  char *result = NULL;

  char published[ISO_TIME_LEN+1];
  char identity64[BASE64_DIGEST_LEN+1];
  char digest64[BASE64_DIGEST_LEN+1];
  smartlist_t *chunks = smartlist_new();

  format_iso_time(published, rs->published_on);
  digest_to_base64(identity64, rs->identity_digest);
  digest_to_base64(digest64, rs->descriptor_digest);

  smartlist_add_asprintf(chunks,
                   "r %s %s %s%s%s %s %d %d\n",
                   rs->nickname,
                   identity64,
                   (format==NS_V3_CONSENSUS_MICRODESC)?"":digest64,
                   (format==NS_V3_CONSENSUS_MICRODESC)?"":" ",
                   published,
                   fmt_addr32(rs->addr),
                   (int)rs->or_port,
                   (int)rs->dir_port);

  /* TODO: Maybe we want to pass in what we need to build the rest of
   * this here, instead of in the caller. Then we could use the
   * networkstatus_type_t values, with an additional control port value
   * added -MP */

  /* V3 microdesc consensuses don't have "a" lines. */
  if (format == NS_V3_CONSENSUS_MICRODESC)
    goto done;

  /* Possible "a" line. At most one for now. */
  if (!tor_addr_is_null(&rs->ipv6_addr)) {
    smartlist_add_asprintf(chunks, "a %s\n",
                           fmt_addrport(&rs->ipv6_addr, rs->ipv6_orport));
  }

  if (format == NS_V3_CONSENSUS)
    goto done;

  smartlist_add_asprintf(chunks,
                   "s%s%s%s%s%s%s%s%s%s%s\n",
                  /* These must stay in alphabetical order. */
                   rs->is_authority?" Authority":"",
                   rs->is_bad_exit?" BadExit":"",
                   rs->is_exit?" Exit":"",
                   rs->is_fast?" Fast":"",
                   rs->is_possible_guard?" Guard":"",
                   rs->is_hs_dir?" HSDir":"",
                   rs->is_flagged_running?" Running":"",
                   rs->is_stable?" Stable":"",
                   rs->is_v2_dir?" V2Dir":"",
                   rs->is_valid?" Valid":"");

  /* length of "opt v \n" */
#define V_LINE_OVERHEAD 7
  if (version && strlen(version) < MAX_V_LINE_LEN - V_LINE_OVERHEAD) {
    smartlist_add_asprintf(chunks, "v %s\n", version);
  }
  if (protocols) {
    smartlist_add_asprintf(chunks, "pr %s\n", protocols);
  }

  if (format != NS_V2) {
    const routerinfo_t* desc = router_get_by_id_digest(rs->identity_digest);
    uint32_t bw_kb;

    if (format != NS_CONTROL_PORT) {
      /* Blow up more or less nicely if we didn't get anything or not the
       * thing we expected.
       */
      if (!desc) {
        char id[HEX_DIGEST_LEN+1];
        char dd[HEX_DIGEST_LEN+1];

        base16_encode(id, sizeof(id), rs->identity_digest, DIGEST_LEN);
        base16_encode(dd, sizeof(dd), rs->descriptor_digest, DIGEST_LEN);
        log_warn(LD_BUG, "Cannot get any descriptor for %s "
            "(wanted descriptor %s).",
            id, dd);
        goto err;
      }

      /* This assert could fire for the control port, because
       * it can request NS documents before all descriptors
       * have been fetched. Therefore, we only do this test when
       * format != NS_CONTROL_PORT. */
      if (tor_memneq(desc->cache_info.signed_descriptor_digest,
            rs->descriptor_digest,
            DIGEST_LEN)) {
        char rl_d[HEX_DIGEST_LEN+1];
        char rs_d[HEX_DIGEST_LEN+1];
        char id[HEX_DIGEST_LEN+1];

        base16_encode(rl_d, sizeof(rl_d),
            desc->cache_info.signed_descriptor_digest, DIGEST_LEN);
        base16_encode(rs_d, sizeof(rs_d), rs->descriptor_digest, DIGEST_LEN);
        base16_encode(id, sizeof(id), rs->identity_digest, DIGEST_LEN);
        log_err(LD_BUG, "descriptor digest in routerlist does not match "
            "the one in routerstatus: %s vs %s "
            "(router %s)\n",
            rl_d, rs_d, id);

        tor_assert(tor_memeq(desc->cache_info.signed_descriptor_digest,
              rs->descriptor_digest,
              DIGEST_LEN));
      }
    }

    if (format == NS_CONTROL_PORT && rs->has_bandwidth) {
      bw_kb = rs->bandwidth_kb;
    } else {
      tor_assert(desc);
      bw_kb = router_get_advertised_bandwidth_capped(desc) / 1000;
    }
    smartlist_add_asprintf(chunks,
                     "w Bandwidth=%d", bw_kb);

    if (format == NS_V3_VOTE && vrs && vrs->has_measured_bw) {
      smartlist_add_asprintf(chunks,
                       " Measured=%d", vrs->measured_bw_kb);
    }
    /* Write down guardfraction information if we have it. */
    if (format == NS_V3_VOTE && vrs && vrs->status.has_guardfraction) {
      smartlist_add_asprintf(chunks,
                             " GuardFraction=%d",
                             vrs->status.guardfraction_percentage);
    }

    smartlist_add(chunks, tor_strdup("\n"));

    if (desc) {
      summary = policy_summarize(desc->exit_policy, AF_INET);
      smartlist_add_asprintf(chunks, "p %s\n", summary);
      tor_free(summary);
    }

    if (format == NS_V3_VOTE && vrs) {
      if (tor_mem_is_zero((char*)vrs->ed25519_id, ED25519_PUBKEY_LEN)) {
        smartlist_add(chunks, tor_strdup("id ed25519 none\n"));
      } else {
        char ed_b64[BASE64_DIGEST256_LEN+1];
        digest256_to_base64(ed_b64, (const char*)vrs->ed25519_id);
        smartlist_add_asprintf(chunks, "id ed25519 %s\n", ed_b64);
      }
    }
  }

 done:
  result = smartlist_join_strings(chunks, "", 0, NULL);

 err:
  SMARTLIST_FOREACH(chunks, char *, cp, tor_free(cp));
  smartlist_free(chunks);

  return result;
}