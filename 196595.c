grub_crypto_hmac_buffer (const struct gcry_md_spec *md,
			 const void *key, grub_size_t keylen,
			 const void *data, grub_size_t datalen, void *out)
{
  struct grub_crypto_hmac_handle *hnd;

  hnd = grub_crypto_hmac_init (md, key, keylen);
  if (!hnd)
    return GPG_ERR_OUT_OF_MEMORY;

  grub_crypto_hmac_write (hnd, data, datalen);
  return grub_crypto_hmac_fini (hnd, out);
}