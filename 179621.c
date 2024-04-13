make_preamble (uint8_t * uint64_data, uint8_t type, unsigned int length,
               uint8_t ver, uint8_t * preamble)
{
  uint8_t minor = _gnutls_version_get_minor (ver);
  uint8_t major = _gnutls_version_get_major (ver);
  uint8_t *p = preamble;
  uint16_t c_length;

  c_length = _gnutls_conv_uint16 (length);

  memcpy (p, uint64_data, 8);
  p += 8;
  *p = type;
  p++;
  if (ver != GNUTLS_SSL3)
    { /* TLS protocols */
      *p = major;
      p++;
      *p = minor;
      p++;
    }
  memcpy (p, &c_length, 2);
  p += 2;
  return p - preamble;
}