grub_crypto_cipher_set_key (grub_crypto_cipher_handle_t cipher,
			    const unsigned char *key,
			    unsigned keylen)
{
  return cipher->cipher->setkey (cipher->ctx, key, keylen);
}