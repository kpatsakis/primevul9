grub_crypto_hash (const gcry_md_spec_t *hash, void *out, const void *in,
		  grub_size_t inlen)
{
  GRUB_PROPERLY_ALIGNED_ARRAY (ctx, GRUB_CRYPTO_MAX_MD_CONTEXT_SIZE);

  if (hash->contextsize > sizeof (ctx))
    grub_fatal ("Too large md context");
  hash->init (&ctx);
  hash->write (&ctx, in, inlen);
  hash->final (&ctx);
  grub_memcpy (out, hash->read (&ctx), hash->mdlen);
}