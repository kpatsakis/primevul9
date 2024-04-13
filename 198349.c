static void mutt_convert_to_state(iconv_t cd, char *bufi, size_t *l, STATE *s)
{
  char bufo[BUFO_SIZE];
  ICONV_CONST char *ib;
  char *ob;
  size_t ibl, obl;

  if (!bufi)
  {
    if (cd != (iconv_t)(-1))
    {
      ob = bufo, obl = sizeof (bufo);
      iconv (cd, 0, 0, &ob, &obl);
      if (ob != bufo)
	state_prefix_put (bufo, ob - bufo, s);
    }
    return;
  }

  if (cd == (iconv_t)(-1))
  {
    state_prefix_put (bufi, *l, s);
    *l = 0;
    return;
  }

  ib = bufi, ibl = *l;
  for (;;)
  {
    ob = bufo, obl = sizeof (bufo);
    mutt_iconv (cd, &ib, &ibl, &ob, &obl, 0, "?");
    if (ob == bufo)
      break;
    state_prefix_put (bufo, ob - bufo, s);
  }
  memmove (bufi, ib, ibl);
  *l = ibl;
}