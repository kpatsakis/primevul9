build_key_sequence (gcry_mpi_t *kparms, int mode, size_t *r_length)
{
  int rc, i;
  size_t needed, n;
  unsigned char *plain, *p;
  size_t plainlen;
  size_t outseqlen, oidseqlen, octstrlen, inseqlen;

  needed = 3; /* The version integer with value 0. */
  for (i=0; kparms[i]; i++)
    {
      n = 0;
      rc = gcry_mpi_print (GCRYMPI_FMT_STD, NULL, 0, &n, kparms[i]);
      if (rc)
        {
          log_error ("error formatting parameter: %s\n", gpg_strerror (rc));
          return NULL;
        }
      needed += n;
      n = compute_tag_length (n);
      if (!n)
        return NULL;
      needed += n;
    }
  if (i != 8)
    {
      log_error ("invalid parameters for p12_build\n");
      return NULL;
    }
  /* Now this all goes into a sequence. */
  inseqlen = needed;
  n = compute_tag_length (needed);
  if (!n)
    return NULL;
  needed += n;

  if (mode != 2)
    {
      /* Encapsulate all into an octet string. */
      octstrlen = needed;
      n = compute_tag_length (needed);
      if (!n)
        return NULL;
      needed += n;
      /* Prepend the object identifier sequence. */
      oidseqlen = 2 + DIM (oid_rsaEncryption) + 2;
      needed += 2 + oidseqlen;
      /* The version number. */
      needed += 3;
      /* And finally put the whole thing into a sequence. */
      outseqlen = needed;
      n = compute_tag_length (needed);
      if (!n)
        return NULL;
      needed += n;
    }

  /* allocate 8 extra bytes for padding */
  plain = gcry_malloc_secure (needed+8);
  if (!plain)
    {
      log_error ("error allocating encryption buffer\n");
      return NULL;
    }

  /* And now fill the plaintext buffer. */
  p = plain;
  if (mode != 2)
    {
      p = store_tag_length (p, TAG_SEQUENCE, outseqlen);
      /* Store version. */
      *p++ = TAG_INTEGER;
      *p++ = 1;
      *p++ = 0;
      /* Store object identifier sequence. */
      p = store_tag_length (p, TAG_SEQUENCE, oidseqlen);
      p = store_tag_length (p, TAG_OBJECT_ID, DIM (oid_rsaEncryption));
      memcpy (p, oid_rsaEncryption, DIM (oid_rsaEncryption));
      p += DIM (oid_rsaEncryption);
      *p++ = TAG_NULL;
      *p++ = 0;
      /* Start with the octet string. */
      p = store_tag_length (p, TAG_OCTET_STRING, octstrlen);
    }

  p = store_tag_length (p, TAG_SEQUENCE, inseqlen);
  /* Store the key parameters. */
  *p++ = TAG_INTEGER;
  *p++ = 1;
  *p++ = 0;
  for (i=0; kparms[i]; i++)
    {
      n = 0;
      rc = gcry_mpi_print (GCRYMPI_FMT_STD, NULL, 0, &n, kparms[i]);
      if (rc)
        {
          log_error ("oops: error formatting parameter: %s\n",
                     gpg_strerror (rc));
          gcry_free (plain);
          return NULL;
        }
      p = store_tag_length (p, TAG_INTEGER, n);

      n = plain + needed - p;
      rc = gcry_mpi_print (GCRYMPI_FMT_STD, p, n, &n, kparms[i]);
      if (rc)
        {
          log_error ("oops: error storing parameter: %s\n",
                     gpg_strerror (rc));
          gcry_free (plain);
          return NULL;
        }
      p += n;
    }

  plainlen = p - plain;
  assert (needed == plainlen);

  if (!mode)
    {
      /* Append some pad characters; we already allocated extra space. */
      n = 8 - plainlen % 8;
      for (i=0; i < n; i++, plainlen++)
        *p++ = n;
    }

  *r_length = plainlen;
  return plain;
}