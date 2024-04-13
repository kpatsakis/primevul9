void rfc822_dequote_comment (char *s)
{
  char *w = s;

  for (; *s; s++)
  {
    if (*s == '\\')
    {
      if (!*++s)
	break; /* error? */
      *w++ = *s;
    }
    else if (*s != '\"')
    {
      if (w != s)
	*w = *s;
      w++;
    }
  }
  *w = 0;
}