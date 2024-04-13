static int is_mmnoask (const char *buf)
{
  char tmp[LONG_STRING], *p, *q;
  int lng;

  if ((p = getenv ("MM_NOASK")) != NULL && *p)
  {
    if (mutt_strcmp (p, "1") == 0)
      return (1);

    strfcpy (tmp, p, sizeof (tmp));
    p = tmp;

    while ((p = strtok (p, ",")) != NULL)
    {
      if ((q = strrchr (p, '/')) != NULL)
      {
	if (*(q+1) == '*')
	{
	  if (ascii_strncasecmp (buf, p, q-p) == 0)
	    return (1);
	}
	else
	{
	  if (ascii_strcasecmp (buf, p) == 0)
	    return (1);
	}
      }
      else
      {
	lng = mutt_strlen (p);
	if (buf[lng] == '/' && mutt_strncasecmp (buf, p, lng) == 0)
	  return (1);
      }

      p = NULL;
    }
  }

  return (0);
}