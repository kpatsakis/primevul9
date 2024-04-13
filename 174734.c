pwg_ppdize_name(const char *ipp,	/* I - IPP keyword */
                char       *name,	/* I - Name buffer */
		size_t     namesize)	/* I - Size of name buffer */
{
  char	*ptr,				/* Pointer into name buffer */
	*end;				/* End of name buffer */


  if (!ipp)
  {
    *name = '\0';
    return;
  }

  *name = (char)toupper(*ipp++);

  for (ptr = name + 1, end = name + namesize - 1; *ipp && ptr < end;)
  {
    if (*ipp == '-' && _cups_isalnum(ipp[1]))
    {
      ipp ++;
      *ptr++ = (char)toupper(*ipp++ & 255);
    }
    else
      *ptr++ = *ipp++;
  }

  *ptr = '\0';
}