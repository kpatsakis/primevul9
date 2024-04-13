pwg_unppdize_name(const char *ppd,	/* I - PPD keyword */
		  char       *name,	/* I - Name buffer */
                  size_t     namesize,	/* I - Size of name buffer */
                  const char *dashchars)/* I - Characters to be replaced by dashes */
{
  char	*ptr,				/* Pointer into name buffer */
	*end;				/* End of name buffer */


  if (_cups_islower(*ppd))
  {
   /*
    * Already lowercase name, use as-is?
    */

    const char *ppdptr;			/* Pointer into PPD keyword */

    for (ppdptr = ppd + 1; *ppdptr; ppdptr ++)
      if (_cups_isupper(*ppdptr) || strchr(dashchars, *ppdptr))
        break;

    if (!*ppdptr)
    {
      strlcpy(name, ppd, namesize);
      return;
    }
  }

  for (ptr = name, end = name + namesize - 1; *ppd && ptr < end; ppd ++)
  {
    if (_cups_isalnum(*ppd) || *ppd == '-')
      *ptr++ = (char)tolower(*ppd & 255);
    else if (strchr(dashchars, *ppd))
      *ptr++ = '-';
    else
      *ptr++ = *ppd;

    if (!_cups_isupper(*ppd) && _cups_isalnum(*ppd) &&
	_cups_isupper(ppd[1]) && ptr < end)
      *ptr++ = '-';
    else if (!isdigit(*ppd & 255) && isdigit(ppd[1] & 255))
      *ptr++ = '-';
  }

  *ptr = '\0';
}