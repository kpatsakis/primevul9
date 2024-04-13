eddsa_encodempi (gcry_mpi_t mpi, unsigned int minlen,
                 unsigned char **r_buffer, unsigned int *r_buflen)
{
  unsigned char *rawmpi;
  unsigned int rawmpilen;

  rawmpi = _gcry_mpi_get_buffer (mpi, minlen, &rawmpilen, NULL);
  if (!rawmpi)
    return gpg_err_code_from_syserror ();

  *r_buffer = rawmpi;
  *r_buflen = rawmpilen;
  return 0;
}