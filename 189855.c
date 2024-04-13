static char* msg_parse_flags (IMAP_HEADER* h, char* s)
{
  IMAP_HEADER_DATA* hd = h->data;

  /* sanity-check string */
  if (ascii_strncasecmp ("FLAGS", s, 5) != 0)
  {
    dprint (1, (debugfile, "msg_parse_flags: not a FLAGS response: %s\n",
      s));
    return NULL;
  }
  s += 5;
  SKIPWS(s);
  if (*s != '(')
  {
    dprint (1, (debugfile, "msg_parse_flags: bogus FLAGS response: %s\n",
      s));
    return NULL;
  }
  s++;

  mutt_free_list (&hd->keywords);
  hd->deleted = hd->flagged = hd->replied = hd->read = hd->old = 0;

  /* start parsing */
  while (*s && *s != ')')
  {
    if (ascii_strncasecmp ("\\deleted", s, 8) == 0)
    {
      s += 8;
      hd->deleted = 1;
    }
    else if (ascii_strncasecmp ("\\flagged", s, 8) == 0)
    {
      s += 8;
      hd->flagged = 1;
    }
    else if (ascii_strncasecmp ("\\answered", s, 9) == 0)
    {
      s += 9;
      hd->replied = 1;
    }
    else if (ascii_strncasecmp ("\\seen", s, 5) == 0)
    {
      s += 5;
      hd->read = 1;
    }
    else if (ascii_strncasecmp ("\\recent", s, 7) == 0)
      s += 7;
    else if (ascii_strncasecmp ("old", s, 3) == 0)
    {
      s += 3;
      hd->old = 1;
    }
    else
    {
      /* store custom flags as well */
      char ctmp;
      char* flag_word = s;

      if (!hd->keywords)
        hd->keywords = mutt_new_list ();

      while (*s && !ISSPACE (*s) && *s != ')')
        s++;
      ctmp = *s;
      *s = '\0';
      mutt_add_list (hd->keywords, flag_word);
      *s = ctmp;
    }
    SKIPWS(s);
  }

  /* wrap up, or note bad flags response */
  if (*s == ')')
    s++;
  else
  {
    dprint (1, (debugfile,
      "msg_parse_flags: Unterminated FLAGS response: %s\n", s));
    return NULL;
  }

  return s;
}