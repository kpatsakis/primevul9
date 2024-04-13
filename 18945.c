dir_signing_key_is_trusted(crypto_pk_env_t *key)
{
  char digest[DIGEST_LEN];
  if (!key) return 0;
  if (crypto_pk_get_digest(key, digest) < 0) {
    log_warn(LD_DIR, "Error computing dir-signing-key digest");
    return 0;
  }
  if (!router_digest_is_trusted_dir(digest)) {
    log_warn(LD_DIR, "Listed dir-signing-key is not trusted");
    return 0;
  }
  return 1;
}