grub_crypto_memcmp (const void *a, const void *b, grub_size_t n)
{
  register grub_size_t counter = 0;
  const grub_uint8_t *pa, *pb;

  for (pa = a, pb = b; n; pa++, pb++, n--)
    {
      if (*pa != *pb)
	counter++;
    }

  return !!counter;
}