grub_crypto_hmac_write (struct grub_crypto_hmac_handle *hnd,
			const void *data,
			grub_size_t datalen)
{
  hnd->md->write (hnd->ctx, data, datalen);
}