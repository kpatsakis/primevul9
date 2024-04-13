_gcry_ecc_eddsa_decodepoint (gcry_mpi_t pk, mpi_ec_t ctx, mpi_point_t result,
                             unsigned char **r_encpk, unsigned int *r_encpklen)
{
  gpg_err_code_t rc;
  unsigned char *rawmpi;
  unsigned int rawmpilen;
  int sign;

  if (mpi_is_opaque (pk))
    {
      const unsigned char *buf;

      buf = mpi_get_opaque (pk, &rawmpilen);
      if (!buf)
        return GPG_ERR_INV_OBJ;
      rawmpilen = (rawmpilen + 7)/8;

      /* Handle compression prefixes.  The size of the buffer will be
         odd in this case.  */
      if (rawmpilen > 1 && (rawmpilen%2))
        {
          /* First check whether the public key has been given in
             standard uncompressed format (SEC1).  No need to recover
             x in this case.  */
          if (buf[0] == 0x04)
            {
              gcry_mpi_t x, y;

              rc = _gcry_mpi_scan (&x, GCRYMPI_FMT_STD,
                                   buf+1, (rawmpilen-1)/2, NULL);
              if (rc)
                return rc;
              rc = _gcry_mpi_scan (&y, GCRYMPI_FMT_STD,
                                   buf+1+(rawmpilen-1)/2, (rawmpilen-1)/2,NULL);
              if (rc)
                {
                  mpi_free (x);
                  return rc;
                }

              if (r_encpk)
                {
                  rc = eddsa_encode_x_y (x, y, ctx->nbits/8, 0,
                                         r_encpk, r_encpklen);
                  if (rc)
                    {
                      mpi_free (x);
                      mpi_free (y);
                      return rc;
                    }
                }
              mpi_snatch (result->x, x);
              mpi_snatch (result->y, y);
              mpi_set_ui (result->z, 1);
              return 0;
            }

          /* Check whether the public key has been prefixed with a 0x40
             byte to explicitly indicate compressed format using a SEC1
             alike prefix byte.  This is a Libgcrypt extension.  */
          if (buf[0] == 0x40)
            {
              rawmpilen--;
              buf++;
            }
        }

      /* EdDSA compressed point.  */
      rawmpi = xtrymalloc (rawmpilen? rawmpilen:1);
      if (!rawmpi)
        return gpg_err_code_from_syserror ();
      memcpy (rawmpi, buf, rawmpilen);
      reverse_buffer (rawmpi, rawmpilen);
    }
  else
    {
      /* Note: Without using an opaque MPI it is not reliable possible
         to find out whether the public key has been given in
         uncompressed format.  Thus we expect native EdDSA format.  */
      rawmpi = _gcry_mpi_get_buffer (pk, ctx->nbits/8, &rawmpilen, NULL);
      if (!rawmpi)
        return gpg_err_code_from_syserror ();
    }

  if (rawmpilen)
    {
      sign = !!(rawmpi[0] & 0x80);
      rawmpi[0] &= 0x7f;
    }
  else
    sign = 0;
  _gcry_mpi_set_buffer (result->y, rawmpi, rawmpilen, 0);
  if (r_encpk)
    {
      /* Revert to little endian.  */
      if (sign && rawmpilen)
        rawmpi[0] |= 0x80;
      reverse_buffer (rawmpi, rawmpilen);
      *r_encpk = rawmpi;
      if (r_encpklen)
        *r_encpklen = rawmpilen;
    }
  else
    xfree (rawmpi);

  rc = _gcry_ecc_eddsa_recover_x (result->x, result->y, sign, ctx);
  mpi_set_ui (result->z, 1);

  return rc;
}