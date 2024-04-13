build_cert_sequence (const unsigned char *buffer, size_t buflen,
                     const unsigned char *sha1hash, const char *keyidstr,
                     size_t *r_length)
{
  size_t len[8], needed, n;
  unsigned char *p, *certseq;
  size_t certseqlen;
  int i;

  assert (strlen (keyidstr) == 8);

  /* Walk 8 steps down to collect the info: */

  /* 7. The data goes into an octet string. */
  needed = compute_tag_length (buflen);
  needed += buflen;

  /* 6. A [0] tag. */
  len[6] = needed;
  needed += compute_tag_length (needed);

  /* 5. An OID. */
  needed += 2 + DIM (oid_x509Certificate_for_pkcs_12);

  /* 4. A sequence. */
  len[4] = needed;
  needed += compute_tag_length (needed);

  /* 3. A [0] tag. */
  len[3] = needed;
  needed += compute_tag_length (needed);

  /* 2b. The attributes which are appended at the end. */
  if (sha1hash)
    needed += DIM (data_attrtemplate) + 20;

  /* 2. An OID. */
  needed += 2 + DIM (oid_pkcs_12_CertBag);

  /* 1. A sequence. */
  len[1] = needed;
  needed += compute_tag_length (needed);

  /* 0. The first sequence. */
  len[0] = needed;
  needed += compute_tag_length (needed);

  /* Now that we have all length information, allocate a buffer. */
  p = certseq = gcry_malloc (needed + 8 /*(for padding)*/);
  if (!certseq)
    {
      log_error ("error allocating buffer\n");
      return NULL;
    }

  /* Walk 8 steps up to store the data. */

  /* 0. Store the first sequence. */
  p = store_tag_length (p, TAG_SEQUENCE, len[0]);

  /* 1. Store the second sequence. */
  p = store_tag_length (p, TAG_SEQUENCE, len[1]);

  /* 2. Store the pkcs12-cert-bag OID. */
  p = store_tag_length (p, TAG_OBJECT_ID, DIM (oid_pkcs_12_CertBag));
  memcpy (p, oid_pkcs_12_CertBag, DIM (oid_pkcs_12_CertBag));
  p += DIM (oid_pkcs_12_CertBag);

  /* 3. Store a [0] tag. */
  p = store_tag_length (p, 0xa0, len[3]);

  /* 4. Store a sequence. */
  p = store_tag_length (p, TAG_SEQUENCE, len[4]);

  /* 5. Store the x509Certificate OID. */
  p = store_tag_length (p, TAG_OBJECT_ID,
                        DIM (oid_x509Certificate_for_pkcs_12));
  memcpy (p, oid_x509Certificate_for_pkcs_12,
          DIM (oid_x509Certificate_for_pkcs_12));
  p += DIM (oid_x509Certificate_for_pkcs_12);

  /* 6. Store a [0] tag. */
  p = store_tag_length (p, 0xa0, len[6]);

  /* 7. And the octet string with the actual certificate. */
  p = store_tag_length (p, TAG_OCTET_STRING, buflen);
  memcpy (p, buffer, buflen);
  p += buflen;

  /* Append the attributes whose length we calculated at step 2b. */
  if (sha1hash)
    {
      memcpy (p, data_attrtemplate, DIM (data_attrtemplate));
      for (i=0; i < 8; i++)
        p[DATA_ATTRTEMPLATE_KEYID_OFF+2*i+1] = keyidstr[i];
      p += DIM (data_attrtemplate);
      memcpy (p, sha1hash, 20);
      p += 20;
    }

  certseqlen = p - certseq;
  if (needed != certseqlen)
    log_debug ("length mismatch: %lu, %lu\n",
               (unsigned long)needed, (unsigned long)certseqlen);

  /* Append some pad characters; we already allocated extra space. */
  n = 8 - certseqlen % 8;
  for (i=0; i < n; i++, certseqlen++)
    *p++ = n;

  *r_length = certseqlen;
  return certseq;
}