build_key_bag (unsigned char *buffer, size_t buflen, char *salt,
               const unsigned char *sha1hash, const char *keyidstr,
               size_t *r_length)
{
  size_t len[11], needed;
  unsigned char *p, *keybag;
  size_t keybaglen;

  /* Walk 11 steps down to collect the info: */

  /* 10. The data goes into an octet string. */
  needed = compute_tag_length (buflen);
  needed += buflen;

  /* 9. Prepend the algorithm identifier. */
  needed += DIM (data_3desiter2048);

  /* 8. Put a sequence around. */
  len[8] = needed;
  needed += compute_tag_length (needed);

  /* 7. Prepend a [0] tag. */
  len[7] = needed;
  needed += compute_tag_length (needed);

  /* 6b. The attributes which are appended at the end. */
  if (sha1hash)
    needed += DIM (data_attrtemplate) + 20;

  /* 6. Prepend the shroudedKeyBag OID. */
  needed += 2 + DIM (oid_pkcs_12_pkcs_8ShroudedKeyBag);

  /* 5+4. Put all into two sequences. */
  len[5] = needed;
  needed += compute_tag_length ( needed);
  len[4] = needed;
  needed += compute_tag_length (needed);

  /* 3. This all goes into an octet string. */
  len[3] = needed;
  needed += compute_tag_length (needed);

  /* 2. Prepend another [0] tag. */
  len[2] = needed;
  needed += compute_tag_length (needed);

  /* 1. Prepend the data OID. */
  needed += 2 + DIM (oid_data);

  /* 0. Prepend another sequence. */
  len[0] = needed;
  needed += compute_tag_length (needed);

  /* Now that we have all length information, allocate a buffer. */
  p = keybag = gcry_malloc (needed);
  if (!keybag)
    {
      log_error ("error allocating buffer\n");
      return NULL;
    }

  /* Walk 11 steps up to store the data. */

  /* 0. Store the first sequence. */
  p = store_tag_length (p, TAG_SEQUENCE, len[0]);

  /* 1. Store the data OID. */
  p = store_tag_length (p, TAG_OBJECT_ID, DIM (oid_data));
  memcpy (p, oid_data, DIM (oid_data));
  p += DIM (oid_data);

  /* 2. Store a [0] tag. */
  p = store_tag_length (p, 0xa0, len[2]);

  /* 3. And an octet string. */
  p = store_tag_length (p, TAG_OCTET_STRING, len[3]);

  /* 4+5. Two sequences. */
  p = store_tag_length (p, TAG_SEQUENCE, len[4]);
  p = store_tag_length (p, TAG_SEQUENCE, len[5]);

  /* 6. Store the shroudedKeyBag OID. */
  p = store_tag_length (p, TAG_OBJECT_ID,
                        DIM (oid_pkcs_12_pkcs_8ShroudedKeyBag));
  memcpy (p, oid_pkcs_12_pkcs_8ShroudedKeyBag,
          DIM (oid_pkcs_12_pkcs_8ShroudedKeyBag));
  p += DIM (oid_pkcs_12_pkcs_8ShroudedKeyBag);

  /* 7. Store a [0] tag. */
  p = store_tag_length (p, 0xa0, len[7]);

  /* 8. Store a sequence. */
  p = store_tag_length (p, TAG_SEQUENCE, len[8]);

  /* 9. Now for the pre-encoded algorithm identifier and the salt. */
  memcpy (p, data_3desiter2048, DIM (data_3desiter2048));
  memcpy (p + DATA_3DESITER2048_SALT_OFF, salt, 8);
  p += DIM (data_3desiter2048);

  /* 10. And the octet string with the encrypted data. */
  p = store_tag_length (p, TAG_OCTET_STRING, buflen);
  memcpy (p, buffer, buflen);
  p += buflen;

  /* Append the attributes whose length we calculated at step 2b. */
  if (sha1hash)
    {
      int i;

      memcpy (p, data_attrtemplate, DIM (data_attrtemplate));
      for (i=0; i < 8; i++)
        p[DATA_ATTRTEMPLATE_KEYID_OFF+2*i+1] = keyidstr[i];
      p += DIM (data_attrtemplate);
      memcpy (p, sha1hash, 20);
      p += 20;
    }


  keybaglen = p - keybag;
  if (needed != keybaglen)
    log_debug ("length mismatch: %lu, %lu\n",
               (unsigned long)needed, (unsigned long)keybaglen);

  *r_length = keybaglen;
  return keybag;
}