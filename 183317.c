parse_bag_encrypted_data (const unsigned char *buffer, size_t length,
                          int startoffset, size_t *r_consumed, const char *pw,
                          void (*certcb)(void*, const unsigned char*, size_t),
                          void *certcbarg, gcry_mpi_t **r_result,
                          int *r_badpass)
{
  struct tag_info ti;
  const unsigned char *p = buffer;
  const unsigned char *p_start = buffer;
  size_t n = length;
  const char *where;
  char salt[20];
  size_t saltlen;
  char iv[16];
  unsigned int iter;
  unsigned char *plain = NULL;
  int bad_pass = 0;
  unsigned char *cram_buffer = NULL;
  size_t consumed = 0; /* Number of bytes consumed from the orginal buffer. */
  int is_3des = 0;
  int is_pbes2 = 0;
  gcry_mpi_t *result = NULL;
  int result_count;

  if (r_result)
    *r_result = NULL;
  where = "start";
  if (parse_tag (&p, &n, &ti))
    goto bailout;
  if (ti.class != ASNCONTEXT || ti.tag)
    goto bailout;
  if (parse_tag (&p, &n, &ti))
    goto bailout;
  if (ti.tag != TAG_SEQUENCE)
    goto bailout;

  where = "bag.encryptedData.version";
  if (parse_tag (&p, &n, &ti))
    goto bailout;
  if (ti.tag != TAG_INTEGER || ti.length != 1 || *p != 0)
    goto bailout;
  p++; n--;
  if (parse_tag (&p, &n, &ti))
    goto bailout;
  if (ti.tag != TAG_SEQUENCE)
    goto bailout;

  where = "bag.encryptedData.data";
  if (parse_tag (&p, &n, &ti))
    goto bailout;
  if (ti.tag != TAG_OBJECT_ID || ti.length != DIM(oid_data)
      || memcmp (p, oid_data, DIM(oid_data)))
    goto bailout;
  p += DIM(oid_data);
  n -= DIM(oid_data);

  where = "bag.encryptedData.keyinfo";
  if (parse_tag (&p, &n, &ti))
    goto bailout;
  if (ti.class || ti.tag != TAG_SEQUENCE)
    goto bailout;
  if (parse_tag (&p, &n, &ti))
    goto bailout;
  if (!ti.class && ti.tag == TAG_OBJECT_ID
      && ti.length == DIM(oid_pbeWithSHAAnd40BitRC2_CBC)
      && !memcmp (p, oid_pbeWithSHAAnd40BitRC2_CBC,
                  DIM(oid_pbeWithSHAAnd40BitRC2_CBC)))
    {
      p += DIM(oid_pbeWithSHAAnd40BitRC2_CBC);
      n -= DIM(oid_pbeWithSHAAnd40BitRC2_CBC);
    }
  else if (!ti.class && ti.tag == TAG_OBJECT_ID
      && ti.length == DIM(oid_pbeWithSHAAnd3_KeyTripleDES_CBC)
      && !memcmp (p, oid_pbeWithSHAAnd3_KeyTripleDES_CBC,
                  DIM(oid_pbeWithSHAAnd3_KeyTripleDES_CBC)))
    {
      p += DIM(oid_pbeWithSHAAnd3_KeyTripleDES_CBC);
      n -= DIM(oid_pbeWithSHAAnd3_KeyTripleDES_CBC);
      is_3des = 1;
    }
  else if (!ti.class && ti.tag == TAG_OBJECT_ID
           && ti.length == DIM(oid_pkcs5PBES2)
           && !memcmp (p, oid_pkcs5PBES2, ti.length))
    {
      p += ti.length;
      n -= ti.length;
      is_pbes2 = 1;
    }
  else
    goto bailout;

  if (is_pbes2)
    {
      where = "pkcs5PBES2-params";
      if (parse_tag (&p, &n, &ti))
        goto bailout;
      if (ti.class || ti.tag != TAG_SEQUENCE)
        goto bailout;
      if (parse_tag (&p, &n, &ti))
        goto bailout;
      if (ti.class || ti.tag != TAG_SEQUENCE)
        goto bailout;
      if (parse_tag (&p, &n, &ti))
        goto bailout;
      if (!(!ti.class && ti.tag == TAG_OBJECT_ID
            && ti.length == DIM(oid_pkcs5PBKDF2)
            && !memcmp (p, oid_pkcs5PBKDF2, ti.length)))
        goto bailout; /* Not PBKDF2.  */
      p += ti.length;
      n -= ti.length;
      if (parse_tag (&p, &n, &ti))
        goto bailout;
      if (ti.class || ti.tag != TAG_SEQUENCE)
        goto bailout;
      if (parse_tag (&p, &n, &ti))
        goto bailout;
      if (!(!ti.class && ti.tag == TAG_OCTET_STRING
            && ti.length >= 8 && ti.length < sizeof salt))
        goto bailout;  /* No salt or unsupported length.  */
      saltlen = ti.length;
      memcpy (salt, p, saltlen);
      p += saltlen;
      n -= saltlen;

      if (parse_tag (&p, &n, &ti))
        goto bailout;
      if (!(!ti.class && ti.tag == TAG_INTEGER && ti.length))
        goto bailout;  /* No valid iteration count.  */
      for (iter=0; ti.length; ti.length--)
        {
          iter <<= 8;
          iter |= (*p++) & 0xff;
          n--;
        }
      /* Note: We don't support the optional parameters but assume
         that the algorithmIdentifier follows. */
      if (parse_tag (&p, &n, &ti))
        goto bailout;
      if (ti.class || ti.tag != TAG_SEQUENCE)
        goto bailout;
      if (parse_tag (&p, &n, &ti))
        goto bailout;
      if (!(!ti.class && ti.tag == TAG_OBJECT_ID
            && ti.length == DIM(oid_aes128_CBC)
            && !memcmp (p, oid_aes128_CBC, ti.length)))
        goto bailout; /* Not AES-128.  */
      p += ti.length;
      n -= ti.length;
      if (parse_tag (&p, &n, &ti))
        goto bailout;
      if (!(!ti.class && ti.tag == TAG_OCTET_STRING && ti.length == sizeof iv))
        goto bailout; /* Bad IV.  */
      memcpy (iv, p, sizeof iv);
      p += sizeof iv;
      n -= sizeof iv;
    }
  else
    {
      where = "rc2or3des-params";
      if (parse_tag (&p, &n, &ti))
        goto bailout;
      if (ti.class || ti.tag != TAG_SEQUENCE)
        goto bailout;
      if (parse_tag (&p, &n, &ti))
        goto bailout;
      if (ti.class || ti.tag != TAG_OCTET_STRING
          || ti.length < 8 || ti.length > 20 )
        goto bailout;
      saltlen = ti.length;
      memcpy (salt, p, saltlen);
      p += saltlen;
      n -= saltlen;
      if (parse_tag (&p, &n, &ti))
        goto bailout;
      if (ti.class || ti.tag != TAG_INTEGER || !ti.length )
        goto bailout;
      for (iter=0; ti.length; ti.length--)
        {
          iter <<= 8;
          iter |= (*p++) & 0xff;
          n--;
        }
    }

  where = "rc2or3desoraes-ciphertext";
  if (parse_tag (&p, &n, &ti))
    goto bailout;

  consumed = p - p_start;
  if (ti.class == ASNCONTEXT && ti.tag == 0 && ti.is_constructed && ti.ndef)
    {
      /* Mozilla exported certs now come with single byte chunks of
         octect strings.  (Mozilla Firefox 1.0.4).  Arghh. */
      where = "cram-rc2or3des-ciphertext";
      cram_buffer = cram_octet_string ( p, &n, &consumed);
      if (!cram_buffer)
        goto bailout;
      p = p_start = cram_buffer;
      if (r_consumed)
        *r_consumed = consumed;
      r_consumed = NULL; /* Ugly hack to not update that value any further. */
      ti.length = n;
    }
  else if (ti.class == ASNCONTEXT && ti.tag == 0 && ti.length )
    ;
  else
    goto bailout;

  log_info ("%lu bytes of %s encrypted text\n",ti.length,
            is_pbes2?"AES128":is_3des?"3DES":"RC2");

  plain = gcry_malloc_secure (ti.length);
  if (!plain)
    {
      log_error ("error allocating decryption buffer\n");
      goto bailout;
    }
  decrypt_block (p, plain, ti.length, salt, saltlen, iter,
                 iv, is_pbes2?16:0, pw,
                 is_pbes2 ? GCRY_CIPHER_AES128 :
                 is_3des  ? GCRY_CIPHER_3DES : GCRY_CIPHER_RFC2268_40,
                 bag_decrypted_data_p);
  n = ti.length;
  startoffset = 0;
  p_start = p = plain;

  where = "outer.outer.seq";
  if (parse_tag (&p, &n, &ti))
    {
      bad_pass = 1;
      goto bailout;
    }
  if (ti.class || ti.tag != TAG_SEQUENCE)
    {
      bad_pass = 1;
      goto bailout;
    }

  if (parse_tag (&p, &n, &ti))
    {
      bad_pass = 1;
      goto bailout;
    }

  /* Loop over all certificates inside the bag. */
  while (n)
    {
      int iscrlbag = 0;
      int iskeybag = 0;

      where = "certbag.nextcert";
      if (ti.class || ti.tag != TAG_SEQUENCE)
        goto bailout;

      where = "certbag.objectidentifier";
      if (parse_tag (&p, &n, &ti))
        goto bailout;
      if (ti.class || ti.tag != TAG_OBJECT_ID)
        goto bailout;
      if ( ti.length == DIM(oid_pkcs_12_CertBag)
           && !memcmp (p, oid_pkcs_12_CertBag, DIM(oid_pkcs_12_CertBag)))
        {
          p += DIM(oid_pkcs_12_CertBag);
          n -= DIM(oid_pkcs_12_CertBag);
        }
      else if ( ti.length == DIM(oid_pkcs_12_CrlBag)
           && !memcmp (p, oid_pkcs_12_CrlBag, DIM(oid_pkcs_12_CrlBag)))
        {
          p += DIM(oid_pkcs_12_CrlBag);
          n -= DIM(oid_pkcs_12_CrlBag);
          iscrlbag = 1;
        }
      else if ( ti.length == DIM(oid_pkcs_12_keyBag)
           && !memcmp (p, oid_pkcs_12_keyBag, DIM(oid_pkcs_12_keyBag)))
        {
          /* The TrustedMIME plugin for MS Outlook started to create
             files with just one outer 3DES encrypted container and
             inside the certificates as well as the key. */
          p += DIM(oid_pkcs_12_keyBag);
          n -= DIM(oid_pkcs_12_keyBag);
          iskeybag = 1;
        }
      else
        goto bailout;

      where = "certbag.before.certheader";
      if (parse_tag (&p, &n, &ti))
        goto bailout;
      if (ti.class != ASNCONTEXT || ti.tag)
        goto bailout;
      if (iscrlbag)
        {
          log_info ("skipping unsupported crlBag\n");
          p += ti.length;
          n -= ti.length;
        }
      else if (iskeybag && (result || !r_result))
        {
          log_info ("one keyBag already processed; skipping this one\n");
          p += ti.length;
          n -= ti.length;
        }
      else if (iskeybag)
        {
          int len;

          log_info ("processing simple keyBag\n");

          /* Fixme: This code is duplicated from parse_bag_data.  */
          if (parse_tag (&p, &n, &ti) || ti.class || ti.tag != TAG_SEQUENCE)
            goto bailout;
          if (parse_tag (&p, &n, &ti) || ti.class || ti.tag != TAG_INTEGER
              || ti.length != 1 || *p)
            goto bailout;
          p++; n--;
          if (parse_tag (&p, &n, &ti) || ti.class || ti.tag != TAG_SEQUENCE)
            goto bailout;
          len = ti.length;
          if (parse_tag (&p, &n, &ti))
            goto bailout;
          if (len < ti.nhdr)
            goto bailout;
          len -= ti.nhdr;
          if (ti.class || ti.tag != TAG_OBJECT_ID
              || ti.length != DIM(oid_rsaEncryption)
              || memcmp (p, oid_rsaEncryption,
                         DIM(oid_rsaEncryption)))
            goto bailout;
          p += DIM (oid_rsaEncryption);
          n -= DIM (oid_rsaEncryption);
          if (len < ti.length)
            goto bailout;
          len -= ti.length;
          if (n < len)
            goto bailout;
          p += len;
          n -= len;
          if ( parse_tag (&p, &n, &ti)
               || ti.class || ti.tag != TAG_OCTET_STRING)
            goto bailout;
          if ( parse_tag (&p, &n, &ti)
               || ti.class || ti.tag != TAG_SEQUENCE)
            goto bailout;
          len = ti.length;

          result = gcry_calloc (10, sizeof *result);
          if (!result)
            {
              log_error ( "error allocating result array\n");
              goto bailout;
            }
          result_count = 0;

          where = "reading.keybag.key-parameters";
          for (result_count = 0; len && result_count < 9;)
            {
              if ( parse_tag (&p, &n, &ti)
                   || ti.class || ti.tag != TAG_INTEGER)
                goto bailout;
              if (len < ti.nhdr)
                goto bailout;
              len -= ti.nhdr;
              if (len < ti.length)
                goto bailout;
              len -= ti.length;
              if (!result_count && ti.length == 1 && !*p)
                ; /* ignore the very first one if it is a 0 */
              else
                {
                  int rc;

                  rc = gcry_mpi_scan (result+result_count, GCRYMPI_FMT_USG, p,
                                      ti.length, NULL);
                  if (rc)
                    {
                      log_error ("error parsing key parameter: %s\n",
                                 gpg_strerror (rc));
                      goto bailout;
                    }
                  result_count++;
                }
              p += ti.length;
              n -= ti.length;
            }
          if (len)
            goto bailout;
        }
      else
        {
          log_info ("processing certBag\n");
          if (parse_tag (&p, &n, &ti))
            goto bailout;
          if (ti.class || ti.tag != TAG_SEQUENCE)
            goto bailout;
          if (parse_tag (&p, &n, &ti))
            goto bailout;
          if (ti.class || ti.tag != TAG_OBJECT_ID
              || ti.length != DIM(oid_x509Certificate_for_pkcs_12)
              || memcmp (p, oid_x509Certificate_for_pkcs_12,
                         DIM(oid_x509Certificate_for_pkcs_12)))
            goto bailout;
          p += DIM(oid_x509Certificate_for_pkcs_12);
          n -= DIM(oid_x509Certificate_for_pkcs_12);

          where = "certbag.before.octetstring";
          if (parse_tag (&p, &n, &ti))
            goto bailout;
          if (ti.class != ASNCONTEXT || ti.tag)
            goto bailout;
          if (parse_tag (&p, &n, &ti))
            goto bailout;
          if (ti.class || ti.tag != TAG_OCTET_STRING || ti.ndef)
            goto bailout;

          /* Return the certificate. */
          if (certcb)
            certcb (certcbarg, p, ti.length);

          p += ti.length;
          n -= ti.length;
        }

      /* Ugly hack to cope with the padding: Forget about the rest if
         that is less or equal to the cipher's block length.  We can
         reasonable assume that all valid data will be longer than
         just one block. */
      if (n <= (is_pbes2? 16:8))
        n = 0;

      /* Skip the optional SET with the pkcs12 cert attributes. */
      if (n)
        {
          where = "bag.attributes";
          if (parse_tag (&p, &n, &ti))
            goto bailout;
          if (!ti.class && ti.tag == TAG_SEQUENCE)
            ; /* No attributes. */
          else if (!ti.class && ti.tag == TAG_SET && !ti.ndef)
            { /* The optional SET. */
              p += ti.length;
              n -= ti.length;
              if (n <= (is_pbes2?16:8))
                n = 0;
              if (n && parse_tag (&p, &n, &ti))
                goto bailout;
            }
          else
            goto bailout;
        }
    }

  if (r_consumed)
    *r_consumed = consumed;
  gcry_free (plain);
  gcry_free (cram_buffer);
  if (r_result)
    *r_result = result;
  return 0;

 bailout:
  if (result)
    {
      int i;

      for (i=0; result[i]; i++)
        gcry_mpi_release (result[i]);
      gcry_free (result);
    }
  if (r_consumed)
    *r_consumed = consumed;
  gcry_free (plain);
  gcry_free (cram_buffer);
  log_error ("encryptedData error at \"%s\", offset %u\n",
             where, (unsigned int)((p - p_start)+startoffset));
  if (bad_pass)
    {
      /* Note, that the following string might be used by other programs
         to check for a bad passphrase; it should therefore not be
         translated or changed. */
      log_error ("possibly bad passphrase given\n");
      *r_badpass = 1;
    }
  return -1;
}