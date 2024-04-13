_gnutls_ecc_ansi_x963_import (const uint8_t * in,
                              unsigned long inlen, bigint_t * x, bigint_t * y)
{
  int ret;

  /* must be odd */
  if ((inlen & 1) == 0)
    {
      return GNUTLS_E_INVALID_REQUEST;
    }

  /* check for 4  */
  if (in[0] != 4)
    {
      return gnutls_assert_val (GNUTLS_E_PARSING_ERROR);
    }

  /* read data */
  ret = _gnutls_mpi_scan (x, in + 1, (inlen - 1) >> 1);
  if (ret < 0)
    return gnutls_assert_val (GNUTLS_E_MEMORY_ERROR);

  ret = _gnutls_mpi_scan (y, in + 1 + ((inlen - 1) >> 1), (inlen - 1) >> 1);
  if (ret < 0)
    {
      _gnutls_mpi_release (x);
      return gnutls_assert_val (GNUTLS_E_MEMORY_ERROR);
    }

  return 0;
}