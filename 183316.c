p12_parse (const unsigned char *buffer, size_t length, const char *pw,
           void (*certcb)(void*, const unsigned char*, size_t),
           void *certcbarg, int *r_badpass)
{
  struct tag_info ti;
  const unsigned char *p = buffer;
  const unsigned char *p_start = buffer;
  size_t n = length;
  const char *where;
  int bagseqlength, len;
  int bagseqndef, lenndef;
  gcry_mpi_t *result = NULL;
  unsigned char *cram_buffer = NULL;

  *r_badpass = 0;
  where = "pfx";
  if (parse_tag (&p, &n, &ti))
    goto bailout;
  if (ti.tag != TAG_SEQUENCE)
    goto bailout;

  where = "pfxVersion";
  if (parse_tag (&p, &n, &ti))
    goto bailout;
  if (ti.tag != TAG_INTEGER || ti.length != 1 || *p != 3)
    goto bailout;
  p++; n--;

  where = "authSave";
  if (parse_tag (&p, &n, &ti))
    goto bailout;
  if (ti.tag != TAG_SEQUENCE)
    goto bailout;
  if (parse_tag (&p, &n, &ti))
    goto bailout;
  if (ti.tag != TAG_OBJECT_ID || ti.length != DIM(oid_data)
      || memcmp (p, oid_data, DIM(oid_data)))
    goto bailout;
  p += DIM(oid_data);
  n -= DIM(oid_data);

  if (parse_tag (&p, &n, &ti))
    goto bailout;
  if (ti.class != ASNCONTEXT || ti.tag)
    goto bailout;
  if (parse_tag (&p, &n, &ti))
    goto bailout;
  if (ti.class != UNIVERSAL || ti.tag != TAG_OCTET_STRING)
    goto bailout;

  if (ti.is_constructed && ti.ndef)
    {
      /* Mozilla exported certs now come with single byte chunks of
         octect strings.  (Mozilla Firefox 1.0.4).  Arghh. */
      where = "cram-bags";
      cram_buffer = cram_octet_string ( p, &n, NULL);
      if (!cram_buffer)
        goto bailout;
      p = p_start = cram_buffer;
    }

  where = "bags";
  if (parse_tag (&p, &n, &ti))
    goto bailout;
  if (ti.class != UNIVERSAL || ti.tag != TAG_SEQUENCE)
    goto bailout;
  bagseqndef = ti.ndef;
  bagseqlength = ti.length;
  while (bagseqlength || bagseqndef)
    {
/*       log_debug ( "at offset %u\n", (p - p_start)); */
      where = "bag-sequence";
      if (parse_tag (&p, &n, &ti))
        goto bailout;
      if (bagseqndef && ti.class == UNIVERSAL && !ti.tag && !ti.is_constructed)
        break; /* Ready */
      if (ti.class != UNIVERSAL || ti.tag != TAG_SEQUENCE)
        goto bailout;

      if (!bagseqndef)
        {
          if (bagseqlength < ti.nhdr)
            goto bailout;
          bagseqlength -= ti.nhdr;
          if (bagseqlength < ti.length)
            goto bailout;
          bagseqlength -= ti.length;
        }
      lenndef = ti.ndef;
      len = ti.length;

      if (parse_tag (&p, &n, &ti))
        goto bailout;
      if (lenndef)
        len = ti.nhdr;
      else
        len -= ti.nhdr;

      if (ti.tag == TAG_OBJECT_ID && ti.length == DIM(oid_encryptedData)
          && !memcmp (p, oid_encryptedData, DIM(oid_encryptedData)))
        {
          size_t consumed = 0;

          p += DIM(oid_encryptedData);
          n -= DIM(oid_encryptedData);
          if (!lenndef)
            len -= DIM(oid_encryptedData);
          where = "bag.encryptedData";
          if (parse_bag_encrypted_data (p, n, (p - p_start), &consumed, pw,
                                        certcb, certcbarg,
                                        result? NULL : &result, r_badpass))
            goto bailout;
          if (lenndef)
            len += consumed;
        }
      else if (ti.tag == TAG_OBJECT_ID && ti.length == DIM(oid_data)
               && !memcmp (p, oid_data, DIM(oid_data)))
        {
          if (result)
            {
              log_info ("already got an key object, skipping this one\n");
              p += ti.length;
              n -= ti.length;
            }
          else
            {
              size_t consumed = 0;

              p += DIM(oid_data);
              n -= DIM(oid_data);
              if (!lenndef)
                len -= DIM(oid_data);
              result = parse_bag_data (p, n, (p - p_start), &consumed, pw);
              if (!result)
                goto bailout;
              if (lenndef)
                len += consumed;
            }
        }
      else
        {
          log_info ("unknown bag type - skipped\n");
          p += ti.length;
          n -= ti.length;
        }

      if (len < 0 || len > n)
        goto bailout;
      p += len;
      n -= len;
      if (lenndef)
        {
          /* Need to skip the Null Tag. */
          if (parse_tag (&p, &n, &ti))
            goto bailout;
          if (!(ti.class == UNIVERSAL && !ti.tag && !ti.is_constructed))
            goto bailout;
        }
    }

  gcry_free (cram_buffer);
  return result;
 bailout:
  log_error ("error at \"%s\", offset %u\n",
             where, (unsigned int)(p - p_start));
  if (result)
    {
      int i;

      for (i=0; result[i]; i++)
        gcry_mpi_release (result[i]);
      gcry_free (result);
    }
  gcry_free (cram_buffer);
  return NULL;
}