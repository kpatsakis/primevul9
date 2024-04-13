_gcry_ecc_eddsa_ensure_compact (gcry_mpi_t value, unsigned int nbits)
{
  gpg_err_code_t rc;
  const unsigned char *buf;
  unsigned int rawmpilen;
  gcry_mpi_t x, y;
  unsigned char *enc;
  unsigned int enclen;

  if (!mpi_is_opaque (value))
    return GPG_ERR_INV_OBJ;
  buf = mpi_get_opaque (value, &rawmpilen);
  if (!buf)
    return GPG_ERR_INV_OBJ;
  rawmpilen = (rawmpilen + 7)/8;

  if (rawmpilen > 1 && (rawmpilen%2))
    {
      if (buf[0] == 0x04)
        {
          /* Buffer is in SEC1 uncompressed format.  Extract y and
             compress.  */
          rc = _gcry_mpi_scan (&x, GCRYMPI_FMT_STD,
                               buf+1, (rawmpilen-1)/2, NULL);
          if (rc)
            return rc;
          rc = _gcry_mpi_scan (&y, GCRYMPI_FMT_STD,
                               buf+1+(rawmpilen-1)/2, (rawmpilen-1)/2, NULL);
          if (rc)
            {
              mpi_free (x);
              return rc;
            }

          rc = eddsa_encode_x_y (x, y, nbits/8, 0, &enc, &enclen);
          mpi_free (x);
          mpi_free (y);
          if (rc)
            return rc;

          mpi_set_opaque (value, enc, 8*enclen);
        }
      else if (buf[0] == 0x40)
        {
          /* Buffer is compressed but with our SEC1 alike compression
             indicator.  Remove that byte.  FIXME: We should write and
             use a function to manipulate an opaque MPI in place. */
          if (!_gcry_mpi_set_opaque_copy (value, buf + 1, (rawmpilen - 1)*8))
            return gpg_err_code_from_syserror ();
        }
    }

  return 0;
}