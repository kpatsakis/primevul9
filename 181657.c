static void ascii_uppercase_to_unicode_le(unsigned char *dest,
                                          const char *src, size_t srclen)
{
  size_t i;
  for(i = 0; i < srclen; i++) {
    dest[2 * i] = (unsigned char)(Curl_raw_toupper(src[i]));
    dest[2 * i + 1] = '\0';
  }
}