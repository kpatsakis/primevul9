asn1_get_length_ber (const unsigned char *ber, int ber_len, int *len)
{
  int ret;
  long err;

  ret = asn1_get_length_der (ber, ber_len, len);
  if (ret == -1 && ber_len > 1)
    {				/* indefinite length method */
      err = _asn1_get_indefinite_length_string (ber + 1, ber_len-1, &ret);
      if (err != ASN1_SUCCESS)
	return -3;
    }

  return ret;
}