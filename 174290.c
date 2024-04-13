_tr46 (const uint8_t * domain_u8, uint8_t ** out, int flags)
{
  size_t len, it;
  uint32_t *domain_u32;
  int err = IDN2_OK, rc;
  int transitional = 0;
  int test_flags;

  if (flags & IDN2_TRANSITIONAL)
    transitional = 1;

  /* convert UTF-8 to UTF-32 */
  if (!(domain_u32 =
       u8_to_u32 (domain_u8, u8_strlen (domain_u8) + 1, NULL, &len)))
    {
      if (errno == ENOMEM)
	return IDN2_MALLOC;
      return IDN2_ENCODING_ERROR;
    }

  size_t len2 = 0;
  for (it = 0; it < len - 1; it++)
    {
      IDNAMap map;

      get_idna_map (domain_u32[it], &map);

      if (map_is (&map, TR46_FLG_DISALLOWED))
	{
	  if (domain_u32[it])
	    {
	      free (domain_u32);
	      return IDN2_DISALLOWED;
	    }
	  len2++;
	}
      else if (map_is (&map, TR46_FLG_MAPPED))
	{
	  len2 += map.nmappings;
	}
      else if (map_is (&map, TR46_FLG_VALID))
	{
	  len2++;
	}
      else if (map_is (&map, TR46_FLG_IGNORED))
	{
	  continue;
	}
      else if (map_is (&map, TR46_FLG_DEVIATION))
	{
	  if (transitional)
	    {
	      len2 += map.nmappings;
	    }
	  else
	    len2++;
	}
      else if (!(flags & IDN2_USE_STD3_ASCII_RULES))
        {
	  if (map_is (&map, TR46_FLG_DISALLOWED_STD3_VALID))
	    {
	      /* valid because UseSTD3ASCIIRules=false, see #TR46 5 */
	      len2++;
	    }
	  else if (map_is (&map, TR46_FLG_DISALLOWED_STD3_MAPPED))
	    {
	      /* mapped because UseSTD3ASCIIRules=false, see #TR46 5 */
	      len2 += map.nmappings;
	    }
        }
    }

  uint32_t *tmp = (uint32_t *) malloc ((len2 + 1) * sizeof (uint32_t));
  if (!tmp)
    {
      free (domain_u32);
      return IDN2_MALLOC;
    }

  len2 = 0;
  for (it = 0; it < len - 1; it++)
    {
      uint32_t c = domain_u32[it];
      IDNAMap map;

      get_idna_map (c, &map);

      if (map_is (&map, TR46_FLG_DISALLOWED))
	{
	  tmp[len2++] = c;
	}
      else if (map_is (&map, TR46_FLG_MAPPED))
	{
	  len2 += get_map_data (tmp + len2, &map);
	}
      else if (map_is (&map, TR46_FLG_VALID))
	{
	  tmp[len2++] = c;
	}
      else if (map_is (&map, TR46_FLG_IGNORED))
	{
	  continue;
	}
      else if (map_is (&map, TR46_FLG_DEVIATION))
	{
	  if (transitional)
	    {
	      len2 += get_map_data (tmp + len2, &map);
	    }
	  else
	    tmp[len2++] = c;
	}
      else if (!(flags & IDN2_USE_STD3_ASCII_RULES))
        {
	  if (map_is (&map, TR46_FLG_DISALLOWED_STD3_VALID))
	    {
	      tmp[len2++] = c;
	    }
	  else if (map_is (&map, TR46_FLG_DISALLOWED_STD3_MAPPED))
	    {
	      len2 += get_map_data (tmp + len2, &map);
	    }
        }
    }
  free (domain_u32);

  /* Normalize to NFC */
  tmp[len2] = 0;
  domain_u32 = u32_normalize (UNINORM_NFC, tmp, len2 + 1, NULL, &len);
  free (tmp);
  tmp = NULL;

  if (!domain_u32)
    {
      if (errno == ENOMEM)
	return IDN2_MALLOC;
      return IDN2_ENCODING_ERROR;
    }

  /* split into labels and check */
  uint32_t *e, *s;
  for (e = s = domain_u32; *e; s = e)
    {
      while (*e && *e != '.')
	e++;

      if (e - s >= 4 && s[0] == 'x' && s[1] == 'n' && s[2] == '-'
	  && s[3] == '-')
	{
	  /* decode punycode and check result non-transitional */
	  size_t ace_len;
	  uint32_t name_u32[IDN2_LABEL_MAX_LENGTH];
	  size_t name_len = IDN2_LABEL_MAX_LENGTH;
	  uint8_t *ace;

	  ace = u32_to_u8 (s + 4, e - s - 4, NULL, &ace_len);
	  if (!ace)
	    {
	      free (domain_u32);
	      if (errno == ENOMEM)
		return IDN2_MALLOC;
	      return IDN2_ENCODING_ERROR;
	    }

	  rc =
	    _idn2_punycode_decode (ace_len, (char *) ace, &name_len, name_u32);

	  free (ace);

	  if (rc)
	    {
	      free (domain_u32);
	      return rc;
	    }

	  test_flags = TR46_NONTRANSITIONAL_CHECK;

	  if (!(flags & IDN2_USE_STD3_ASCII_RULES))
	    test_flags |= TEST_ALLOW_STD3_DISALLOWED;

	  if ((rc =
	       _idn2_label_test (test_flags, name_u32,
				 name_len)))
	    err = rc;
	}
      else
	{
	  test_flags = transitional ? TR46_TRANSITIONAL_CHECK : TR46_NONTRANSITIONAL_CHECK;

	  if (!(flags & IDN2_USE_STD3_ASCII_RULES))
	    test_flags |= TEST_ALLOW_STD3_DISALLOWED;

	  if ((rc =
	       _idn2_label_test (test_flags, s, e - s)))
	    err = rc;
	}

      if (*e)
	e++;
    }

  if (err == IDN2_OK && out)
    {
      uint8_t *_out = u32_to_u8 (domain_u32, len, NULL, &len);
      free (domain_u32);

      if (!_out)
	{
	  if (errno == ENOMEM)
	    return IDN2_MALLOC;
	  return IDN2_ENCODING_ERROR;
	}

      *out = _out;
    }
  else
    free (domain_u32);

  return err;
}