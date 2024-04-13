cram_octet_string (const unsigned char *input, size_t *length,
                   size_t *input_consumed)
{
  const unsigned char *s = input;
  size_t n = *length;
  unsigned char *output, *d;
  struct tag_info ti;

  /* Allocate output buf.  We know that it won't be longer than the
     input buffer. */
  d = output = gcry_malloc (n);
  if (!output)
    goto bailout;

  for (;;)
    {
      if (parse_tag (&s, &n, &ti))
        goto bailout;
      if (ti.class == UNIVERSAL && ti.tag == TAG_OCTET_STRING
          && !ti.ndef && !ti.is_constructed)
        {
          memcpy (d, s, ti.length);
          s += ti.length;
          d += ti.length;
          n -= ti.length;
        }
      else if (ti.class == UNIVERSAL && !ti.tag && !ti.is_constructed)
        break; /* Ready */
      else
        goto bailout;
    }


  *length = d - output;
  if (input_consumed)
    *input_consumed += s - input;
  return output;

 bailout:
  if (input_consumed)
    *input_consumed += s - input;
  gcry_free (output);
  return NULL;
}