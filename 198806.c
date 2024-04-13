rfc822_cat (char *buf, size_t buflen, const char *value, const char *specials)
{
  if (strpbrk (value, specials))
  {
    char tmp[256], *pc = tmp;
    size_t tmplen = sizeof (tmp) - 3;

    *pc++ = '"';
    for (; *value && tmplen > 1; value++)
    {
      if (*value == '\\' || *value == '"')
      {
	*pc++ = '\\';
	tmplen--;
      }
      *pc++ = *value;
      tmplen--;
    }
    *pc++ = '"';
    *pc = 0;
    strfcpy (buf, tmp, buflen);
  }
  else
    strfcpy (buf, value, buflen);
}