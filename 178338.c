_gcry_ecc_eddsa_compute_h_d (unsigned char **r_digest,
                             gcry_mpi_t d, mpi_ec_t ec)
{
  gpg_err_code_t rc;
  unsigned char *rawmpi = NULL;
  unsigned int rawmpilen;
  unsigned char *digest;
  gcry_buffer_t hvec[2];
  int hashalgo, b;

  *r_digest = NULL;

  hashalgo = GCRY_MD_SHA512;
  if (hashalgo != GCRY_MD_SHA512)
    return GPG_ERR_DIGEST_ALGO;

  b = (ec->nbits+7)/8;
  if (b != 256/8)
    return GPG_ERR_INTERNAL; /* We only support 256 bit. */

  /* Note that we clear DIGEST so we can use it as input to left pad
     the key with zeroes for hashing.  */
  digest = xtrycalloc_secure (2, b);
  if (!digest)
    return gpg_err_code_from_syserror ();

  memset (hvec, 0, sizeof hvec);

  rawmpi = _gcry_mpi_get_buffer (d, 0, &rawmpilen, NULL);
  if (!rawmpi)
    {
      xfree (digest);
      return gpg_err_code_from_syserror ();
    }

  hvec[0].data = digest;
  hvec[0].off = 0;
  hvec[0].len = b > rawmpilen? b - rawmpilen : 0;
  hvec[1].data = rawmpi;
  hvec[1].off = 0;
  hvec[1].len = rawmpilen;
  rc = _gcry_md_hash_buffers (hashalgo, 0, digest, hvec, 2);
  xfree (rawmpi);
  if (rc)
    {
      xfree (digest);
      return rc;
    }

  /* Compute the A value.  */
  reverse_buffer (digest, 32);  /* Only the first half of the hash.  */
  digest[0]   = (digest[0] & 0x7f) | 0x40;
  digest[31] &= 0xf8;

  *r_digest = digest;
  return 0;
}