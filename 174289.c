idn2_lookup_ul (const char * src, char ** lookupname, int flags)
{
  uint8_t *utf8src = NULL;
  int rc;

  if (src)
    {
      const char *encoding = locale_charset ();

      utf8src = u8_strconv_from_encoding (src, encoding, iconveh_error);

      if (!utf8src)
	{
	  if (errno == ENOMEM)
	    return IDN2_MALLOC;
	  return IDN2_ICONV_FAIL;
	}
    }

  rc = idn2_lookup_u8 (utf8src, (uint8_t **) lookupname,
		       flags | IDN2_NFC_INPUT);

  free (utf8src);

  return rc;
}