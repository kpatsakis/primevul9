grub_crypto_cbc_encrypt (grub_crypto_cipher_handle_t cipher,
			 void *out, const void *in, grub_size_t size,
			 void *iv_in)
{
  grub_uint8_t *outptr;
  const grub_uint8_t *inptr, *end;
  void *iv;
  grub_size_t blocksize;
  if (!cipher->cipher->encrypt)
    return GPG_ERR_NOT_SUPPORTED;
  blocksize = cipher->cipher->blocksize;
  if (blocksize == 0 || (((blocksize - 1) & blocksize) != 0)
      || ((size & (blocksize - 1)) != 0))
    return GPG_ERR_INV_ARG;
  end = (const grub_uint8_t *) in + size;
  iv = iv_in;
  for (inptr = in, outptr = out; inptr < end;
       inptr += blocksize, outptr += blocksize)
    {
      grub_crypto_xor (outptr, inptr, iv, blocksize);
      cipher->cipher->encrypt (cipher->ctx, outptr, outptr);
      iv = outptr;
    }
  grub_memcpy (iv_in, iv, blocksize);
  return GPG_ERR_NO_ERROR;
}