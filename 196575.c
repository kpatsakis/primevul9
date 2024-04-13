grub_cipher_register (gcry_cipher_spec_t *cipher)
{
  cipher->next = grub_ciphers;
  grub_ciphers = cipher;
}