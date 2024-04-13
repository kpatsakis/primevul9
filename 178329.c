eddsa_encode_x_y (gcry_mpi_t x, gcry_mpi_t y, unsigned int minlen,
                  int with_prefix,
                  unsigned char **r_buffer, unsigned int *r_buflen)
{
  unsigned char *rawmpi;
  unsigned int rawmpilen;
  int off = with_prefix? 1:0;

  rawmpi = _gcry_mpi_get_buffer_extra (y, minlen, off?-1:0, &rawmpilen, NULL);
  if (!rawmpi)
    return gpg_err_code_from_syserror ();
  if (mpi_test_bit (x, 0) && rawmpilen)
    rawmpi[off + rawmpilen - 1] |= 0x80;  /* Set sign bit.  */
  if (off)
    rawmpi[0] = 0x40;

  *r_buffer = rawmpi;
  *r_buflen = rawmpilen + off;
  return 0;
}