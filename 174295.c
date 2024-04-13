idn2_lookup_u8 (const uint8_t * src, uint8_t ** lookupname, int flags)
{
  size_t lookupnamelen = 0;
  uint8_t _lookupname[IDN2_DOMAIN_MAX_LENGTH + 1];
  uint8_t _mapped[IDN2_DOMAIN_MAX_LENGTH + 1];
  int rc;

  if (src == NULL)
    {
      if (lookupname)
	*lookupname = NULL;
      return IDN2_OK;
    }

  rc = set_default_flags(&flags);
  if (rc != IDN2_OK)
    return rc;

  if (!(flags & IDN2_NO_TR46))
    {
      uint8_t *out;
      size_t outlen;

      rc = _tr46 (src, &out, flags);
      if (rc != IDN2_OK)
	return rc;

      outlen = u8_strlen (out);
      if (outlen >= sizeof (_mapped))
	{
	  free (out);
	  return IDN2_TOO_BIG_DOMAIN;
	}

      memcpy (_mapped, out, outlen + 1);
      src = _mapped;
      free (out);
    }

  do
    {
      const uint8_t *end = (uint8_t *) strchrnul ((const char *) src, '.');
      /* XXX Do we care about non-U+002E dots such as U+3002, U+FF0E
         and U+FF61 here?  Perhaps when IDN2_NFC_INPUT? */
      size_t labellen = end - src;
      uint8_t tmp[IDN2_LABEL_MAX_LENGTH];
      size_t tmplen = IDN2_LABEL_MAX_LENGTH;

      rc = label (src, labellen, tmp, &tmplen, flags);
      if (rc != IDN2_OK)
	return rc;

      if (lookupnamelen + tmplen
	  > IDN2_DOMAIN_MAX_LENGTH - (tmplen == 0 && *end == '\0' ? 1 : 2))
	return IDN2_TOO_BIG_DOMAIN;

      memcpy (_lookupname + lookupnamelen, tmp, tmplen);
      lookupnamelen += tmplen;

      if (*end == '.')
	{
	  if (lookupnamelen + 1 > IDN2_DOMAIN_MAX_LENGTH)
	    return IDN2_TOO_BIG_DOMAIN;

	  _lookupname[lookupnamelen] = '.';
	  lookupnamelen++;
	}
      _lookupname[lookupnamelen] = '\0';

      src = end;
    }
  while (*src++);

  if (lookupname)
    {
      uint8_t *tmp = (uint8_t *) malloc (lookupnamelen + 1);

      if (tmp == NULL)
	return IDN2_MALLOC;

      memcpy (tmp, _lookupname, lookupnamelen + 1);
      *lookupname = tmp;
    }

  return IDN2_OK;
}