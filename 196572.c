grub_crypto_hmac_fini (struct grub_crypto_hmac_handle *hnd, void *out)
{
  grub_uint8_t *p;
  grub_uint8_t *ctx2;

  ctx2 = grub_malloc (hnd->md->contextsize);
  if (!ctx2)
    return GPG_ERR_OUT_OF_MEMORY;

  hnd->md->final (hnd->ctx);
  hnd->md->read (hnd->ctx);
  p = hnd->md->read (hnd->ctx);

  hnd->md->init (ctx2);
  hnd->md->write (ctx2, hnd->opad, hnd->md->blocksize);
  hnd->md->write (ctx2, p, hnd->md->mdlen);
  hnd->md->final (ctx2);
  grub_memset (hnd->opad, 0, hnd->md->blocksize);
  grub_free (hnd->opad);
  grub_memset (hnd->ctx, 0, hnd->md->contextsize);
  grub_free (hnd->ctx);

  grub_memcpy (out, hnd->md->read (ctx2), hnd->md->mdlen);
  grub_memset (ctx2, 0, hnd->md->contextsize);
  grub_free (ctx2);

  grub_memset (hnd, 0, sizeof (*hnd));
  grub_free (hnd);

  return GPG_ERR_NO_ERROR;
}