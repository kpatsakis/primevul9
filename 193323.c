make_preamble (opaque * uint64_data, opaque type, int length,
               opaque ver, opaque * preamble)
{
  opaque minor = _gnutls_version_get_minor (ver);
  opaque major = _gnutls_version_get_major (ver);
  opaque *p = preamble;
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