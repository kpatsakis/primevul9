router_append_dirobj_signature(char *buf, size_t buf_len, const char *digest,
                               size_t digest_len, crypto_pk_env_t *private_key)
{
  char *signature;
  size_t i, keysize;
  int siglen;

  keysize = crypto_pk_keysize(private_key);
  signature = tor_malloc(keysize);
  siglen = crypto_pk_private_sign(private_key, signature, keysize,
                                  digest, digest_len);
  if (siglen < 0) {
    log_warn(LD_BUG,"Couldn't sign digest.");
    goto err;
  }
  if (strlcat(buf, "-----BEGIN SIGNATURE-----\n", buf_len) >= buf_len)
    goto truncated;

  i = strlen(buf);
  if (base64_encode(buf+i, buf_len-i, signature, siglen) < 0) {
    log_warn(LD_BUG,"couldn't base64-encode signature");
    goto err;
  }

  if (strlcat(buf, "-----END SIGNATURE-----\n", buf_len) >= buf_len)
    goto truncated;

  tor_free(signature);
  return 0;

 truncated:
  log_warn(LD_BUG,"tried to exceed string length.");
 err:
  tor_free(signature);
  return -1;
}