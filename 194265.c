asn1_decode_simple_der (unsigned int etype, const unsigned char *der,
			unsigned int _der_len, const unsigned char **str,
			unsigned int *str_len)
{
  return _asn1_decode_simple_der(etype, der, _der_len, str, str_len, DECODE_FLAG_HAVE_TAG);
}