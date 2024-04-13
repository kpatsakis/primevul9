idn2_to_ascii_8z (const char * input, char ** output, int flags)
{
  return idn2_lookup_u8 ((const uint8_t *) input, (uint8_t **) output, flags);
}