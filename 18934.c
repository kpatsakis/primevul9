check_signature_token(const char *digest,
                      ssize_t digest_len,
                      directory_token_t *tok,
                      crypto_pk_env_t *pkey,
                      int flags,
                      const char *doctype)
{
  char *signed_digest;
  size_t keysize;
  const int check_authority = (flags & CST_CHECK_AUTHORITY);
  const int check_objtype = ! (flags & CST_NO_CHECK_OBJTYPE);

  tor_assert(pkey);
  tor_assert(tok);
  tor_assert(digest);
  tor_assert(doctype);

  if (check_authority && !dir_signing_key_is_trusted(pkey)) {
    log_warn(LD_DIR, "Key on %s did not come from an authority; rejecting",
             doctype);
    return -1;
  }

  if (check_objtype) {
    if (strcmp(tok->object_type, "SIGNATURE")) {
      log_warn(LD_DIR, "Bad object type on %s signature", doctype);
      return -1;
    }
  }

  keysize = crypto_pk_keysize(pkey);
  signed_digest = tor_malloc(keysize);
  if (crypto_pk_public_checksig(pkey, signed_digest, keysize,
                                tok->object_body, tok->object_size)
      < digest_len) {
    log_warn(LD_DIR, "Error reading %s: invalid signature.", doctype);
    tor_free(signed_digest);
    return -1;
  }
//  log_debug(LD_DIR,"Signed %s hash starts %s", doctype,
//            hex_str(signed_digest,4));
  if (tor_memneq(digest, signed_digest, digest_len)) {
    log_warn(LD_DIR, "Error reading %s: signature does not match.", doctype);
    tor_free(signed_digest);
    return -1;
  }
  tor_free(signed_digest);
  return 0;
}