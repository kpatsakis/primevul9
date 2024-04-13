grub_crypto_cbc_decrypt (grub_crypto_cipher_handle_t cipher,
			 void *out, const void *in, grub_size_t size,
			 void *iv)
{
  const grub_uint8_t *inptr, *end;
  grub_uint8_t *outptr;
  grub_uint8_t ivt[GRUB_CRYPTO_MAX_CIPHER_BLOCKSIZE];
  grub_size_t blocksize;
  if (!cipher->cipher->decrypt)
    return GPG_ERR_NOT_SUPPORTED;
  blocksize = cipher->cipher->blocksize;
  if (blocksize == 0 || (((blocksize - 1) & blocksize) != 0)
      || ((size & (blocksize - 1)) != 0))
    return GPG_ERR_INV_ARG;
  if (blocksize > GRUB_CRYPTO_MAX_CIPHER_BLOCKSIZE)
    return GPG_ERR_INV_ARG;
  end = (const grub_uint8_t *) in + size;
  for (inptr = in, outptr = out; inptr < end;
       inptr += blocksize, outptr += blocksize)
    {
      grub_memcpy (ivt, inptr, blocksize);
      cipher->cipher->decrypt (cipher->ctx, outptr, inptr);
      grub_crypto_xor (outptr, outptr, iv, blocksize);
      grub_memcpy (iv, ivt, blocksize);
    }
  return GPG_ERR_NO_ERROR;
}