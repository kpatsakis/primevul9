bag_decrypted_data_p (const void *plaintext, size_t length)
{
  struct tag_info ti;
  const unsigned char *p = plaintext;
  size_t n = length;

  /*   { */
  /* #  warning debug code is enabled */
  /*     FILE *fp = fopen ("tmp-rc2-plain.der", "wb"); */
  /*     if (!fp || fwrite (p, n, 1, fp) != 1) */
  /*       exit (2); */
  /*     fclose (fp); */
  /*   } */

  if (parse_tag (&p, &n, &ti))
    return 0;
  if (ti.class || ti.tag != TAG_SEQUENCE)
    return 0;
  if (parse_tag (&p, &n, &ti))
    return 0;

  return 1;
}