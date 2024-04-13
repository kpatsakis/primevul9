build_cert_bag (unsigned char *buffer, size_t buflen, char *salt,
                size_t *r_length)
{
  size_t len[9], needed;
  unsigned char *p, *certbag;
  size_t certbaglen;

  /* Walk 9 steps down to collect the info: */

  /* 8. The data goes into an octet string. */
  needed = compute_tag_length (buflen);
  needed += buflen;

  /* 7. The algorithm identifier. */
  needed += DIM (data_rc2iter2048);

  /* 6. The data OID. */
  needed += 2 + DIM (oid_data);

  /* 5. A sequence. */
  len[5] = needed;
  needed += compute_tag_length ( needed);

  /* 4. An integer. */
  needed += 3;

  /* 3. A sequence. */
  len[3] = needed;
  needed += compute_tag_length (needed);

  /* 2.  A [0] tag. */
  len[2] = needed;
  needed += compute_tag_length (needed);

  /* 1. The encryptedData OID. */
  needed += 2 + DIM (oid_encryptedData);

  /* 0. The first sequence. */
  len[0] = needed;
  needed += compute_tag_length (needed);

  /* Now that we have all length information, allocate a buffer. */
  p = certbag = gcry_malloc (needed);
  if (!certbag)
    {
      log_error ("error allocating buffer\n");
      return NULL;
    }

  /* Walk 9 steps up to store the data. */

  /* 0. Store the first sequence. */
  p = store_tag_length (p, TAG_SEQUENCE, len[0]);

  /* 1. Store the encryptedData OID. */
  p = store_tag_length (p, TAG_OBJECT_ID, DIM (oid_encryptedData));
  memcpy (p, oid_encryptedData, DIM (oid_encryptedData));
  p += DIM (oid_encryptedData);

  /* 2. Store a [0] tag. */
  p = store_tag_length (p, 0xa0, len[2]);

  /* 3. Store a sequence. */
  p = store_tag_length (p, TAG_SEQUENCE, len[3]);

  /* 4. Store the integer 0. */
  *p++ = TAG_INTEGER;
  *p++ = 1;
  *p++ = 0;

  /* 5. Store a sequence. */
  p = store_tag_length (p, TAG_SEQUENCE, len[5]);

  /* 6. Store the data OID. */
  p = store_tag_length (p, TAG_OBJECT_ID, DIM (oid_data));
  memcpy (p, oid_data, DIM (oid_data));
  p += DIM (oid_data);

  /* 7. Now for the pre-encoded algorithm identifier and the salt. */
  memcpy (p, data_rc2iter2048, DIM (data_rc2iter2048));
  memcpy (p + DATA_RC2ITER2048_SALT_OFF, salt, 8);
  p += DIM (data_rc2iter2048);

  /* 8. And finally the [0] tag with the encrypted data. */
  p = store_tag_length (p, 0x80, buflen);
  memcpy (p, buffer, buflen);
  p += buflen;
  certbaglen = p - certbag;

  if (needed != certbaglen)
    log_debug ("length mismatch: %lu, %lu\n",
               (unsigned long)needed, (unsigned long)certbaglen);

  *r_length = certbaglen;
  return certbag;
}