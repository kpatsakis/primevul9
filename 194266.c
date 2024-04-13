_asn1_decode_simple_der (unsigned int etype, const unsigned char *der,
			unsigned int _der_len, const unsigned char **str,
			unsigned int *str_len, unsigned dflags)
{
  int tag_len, len_len;
  const unsigned char *p;
  int der_len = _der_len;
  unsigned char class;
  unsigned long tag;
  long ret;

  if (der == NULL || der_len == 0)
    return ASN1_VALUE_NOT_VALID;

  if (ETYPE_OK (etype) == 0 || ETYPE_IS_STRING(etype) == 0)
    return ASN1_VALUE_NOT_VALID;

  /* doesn't handle constructed classes */
  class = ETYPE_CLASS(etype);
  if (class != ASN1_CLASS_UNIVERSAL)
    return ASN1_VALUE_NOT_VALID;

  p = der;

  if (dflags & DECODE_FLAG_HAVE_TAG)
    {
      ret = asn1_get_tag_der (p, der_len, &class, &tag_len, &tag);
      if (ret != ASN1_SUCCESS)
        return ret;

      if (class != ETYPE_CLASS (etype) || tag != ETYPE_TAG (etype))
        {
          warn();
          return ASN1_DER_ERROR;
        }

      p += tag_len;
      der_len -= tag_len;
      if (der_len <= 0)
        return ASN1_DER_ERROR;
    }

  ret = asn1_get_length_der (p, der_len, &len_len);
  if (ret < 0)
    return ASN1_DER_ERROR;

  p += len_len;
  der_len -= len_len;
  if (der_len <= 0)
    return ASN1_DER_ERROR;

  *str_len = ret;
  *str = p;

  return ASN1_SUCCESS;
}