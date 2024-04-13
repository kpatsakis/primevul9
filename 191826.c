dirserv_router_get_status(const routerinfo_t *router, const char **msg,
                          int severity)
{
  char d[DIGEST_LEN];
  const int key_pinning = get_options()->AuthDirPinKeys;

  if (crypto_pk_get_digest(router->identity_pkey, d)) {
    log_warn(LD_BUG,"Error computing fingerprint");
    if (msg)
      *msg = "Bug: Error computing fingerprint";
    return FP_REJECT;
  }

  /* dirserv_get_status_impl already rejects versions older than 0.2.4.18-rc,
   * and onion_curve25519_pkey was introduced in 0.2.4.8-alpha.
   * But just in case a relay doesn't provide or lies about its version, or
   * doesn't include an ntor key in its descriptor, check that it exists,
   * and is non-zero (clients check that it's non-zero before using it). */
  if (!routerinfo_has_curve25519_onion_key(router)) {
    log_fn(severity, LD_DIR,
           "Descriptor from router %s is missing an ntor curve25519 onion "
           "key.", router_describe(router));
    if (msg)
      *msg = "Missing ntor curve25519 onion key. Please upgrade!";
    return FP_REJECT;
  }

  if (router->cache_info.signing_key_cert) {
    /* This has an ed25519 identity key. */
    if (KEYPIN_MISMATCH ==
        keypin_check((const uint8_t*)router->cache_info.identity_digest,
                   router->cache_info.signing_key_cert->signing_key.pubkey)) {
      log_fn(severity, LD_DIR,
             "Descriptor from router %s has an Ed25519 key, "
               "but the <rsa,ed25519> keys don't match what they were before.",
               router_describe(router));
      if (key_pinning) {
        if (msg) {
          *msg = "Ed25519 identity key or RSA identity key has changed.";
        }
        return FP_REJECT;
      }
    }
  } else {
    /* No ed25519 key */
    if (KEYPIN_MISMATCH == keypin_check_lone_rsa(
                        (const uint8_t*)router->cache_info.identity_digest)) {
      log_fn(severity, LD_DIR,
               "Descriptor from router %s has no Ed25519 key, "
               "when we previously knew an Ed25519 for it. Ignoring for now, "
               "since Ed25519 keys are fairly new.",
               router_describe(router));
#ifdef DISABLE_DISABLING_ED25519
      if (key_pinning) {
        if (msg) {
          *msg = "Ed25519 identity key has disappeared.";
        }
        return FP_REJECT;
      }
#endif
    }
  }

  return dirserv_get_status_impl(d, router->nickname,
                                 router->addr, router->or_port,
                                 router->platform, msg, severity);
}