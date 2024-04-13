p12_raw_build (gcry_mpi_t *kparms, int rawmode, size_t *r_length)
{
  unsigned char *buffer;
  size_t buflen;

  assert (rawmode == 1 || rawmode == 2);
  buffer = build_key_sequence (kparms, rawmode, &buflen);
  if (!buffer)
    return NULL;

  *r_length = buflen;
  return buffer;
}