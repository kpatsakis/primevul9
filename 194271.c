asn1_decode_simple_ber (unsigned int etype, const unsigned char *der,
			unsigned int _der_len, unsigned char **str,
			unsigned int *str_len, unsigned int *ber_len)
{
  return _asn1_decode_simple_ber(etype, der, _der_len, str, str_len, ber_len, DECODE_FLAG_HAVE_TAG);
}