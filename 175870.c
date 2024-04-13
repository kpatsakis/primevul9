str2prefix_rd (const char *str, struct prefix_rd *prd)
{
  int ret; /* ret of called functions */
  int lret; /* local ret, of this func */
  char *p;
  char *p2;
  struct stream *s = NULL;
  char *half = NULL;
  struct in_addr addr;

  s = stream_new (8);

  prd->family = AF_UNSPEC;
  prd->prefixlen = 64;

  lret = 0;
  p = strchr (str, ':');
  if (! p)
    goto out;

  if (! all_digit (p + 1))
    goto out;

  half = XMALLOC (MTYPE_TMP, (p - str) + 1);
  memcpy (half, str, (p - str));
  half[p - str] = '\0';

  p2 = strchr (str, '.');

  if (! p2)
    {
      if (! all_digit (half))
        goto out;
      
      stream_putw (s, RD_TYPE_AS);
      stream_putw (s, atoi (half));
      stream_putl (s, atol (p + 1));
    }
  else
    {
      ret = inet_aton (half, &addr);
      if (! ret)
        goto out;
      
      stream_putw (s, RD_TYPE_IP);
      stream_put_in_addr (s, &addr);
      stream_putw (s, atol (p + 1));
    }
  memcpy (prd->val, s->data, 8);
  lret = 1;

out:
  if (s)
    stream_free (s);
  if (half)
    XFREE(MTYPE_TMP, half);
  return lret;
}