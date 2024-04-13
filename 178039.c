static bool is_ASCII_name(const char *hostname)
{
  const unsigned char *ch = (const unsigned char *)hostname;

  while(*ch) {
    if(*ch++ & 0x80)
      return FALSE;
  }
  return TRUE;
}