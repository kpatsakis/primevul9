idn2_to_ascii_4z (const uint32_t * input, char ** output, int flags)
{
  uint8_t *input_u8;
  size_t length;
  int rc;

  if (!input)
    {
      if (output)
	*output = NULL;
      return IDN2_OK;
    }

  input_u8 = u32_to_u8 (input, u32_strlen(input) + 1, NULL, &length);
  if (!input_u8)
    {
      if (errno == ENOMEM)
	return IDN2_MALLOC;
      return IDN2_ENCODING_ERROR;
    }

  rc = idn2_lookup_u8 (input_u8, (uint8_t **) output, flags);
  free (input_u8);

  return rc;
}