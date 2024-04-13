grub_crypto_hmac_init (const struct gcry_md_spec *md,
		       const void *key, grub_size_t keylen)
{
  grub_uint8_t *helpkey = NULL;
  grub_uint8_t *ipad = NULL, *opad = NULL;
  void *ctx = NULL;
  struct grub_crypto_hmac_handle *ret = NULL;
  unsigned i;

  if (md->mdlen > md->blocksize)
    return NULL;

  ctx = grub_malloc (md->contextsize);
  if (!ctx)
    goto err;

  if ( keylen > md->blocksize ) 
    {
      helpkey = grub_malloc (md->mdlen);
      if (!helpkey)
	goto err;
      grub_crypto_hash (md, helpkey, key, keylen);

      key = helpkey;
      keylen = md->mdlen;
    }

  ipad = grub_zalloc (md->blocksize);
  if (!ipad)
    goto err;

  opad = grub_zalloc (md->blocksize);
  if (!opad)
    goto err;

  grub_memcpy ( ipad, key, keylen );
  grub_memcpy ( opad, key, keylen );
  for (i=0; i < md->blocksize; i++ ) 
    {
      ipad[i] ^= 0x36;
      opad[i] ^= 0x5c;
    }
  grub_free (helpkey);
  helpkey = NULL;

  md->init (ctx);

  md->write (ctx, ipad, md->blocksize); /* inner pad */
  grub_memset (ipad, 0, md->blocksize);
  grub_free (ipad);
  ipad = NULL;

  ret = grub_malloc (sizeof (*ret));
  if (!ret)
    goto err;

  ret->md = md;
  ret->ctx = ctx;
  ret->opad = opad;

  return ret;

 err:
  grub_free (helpkey);
  grub_free (ctx);
  grub_free (ipad);
  grub_free (opad);
  return NULL;
}