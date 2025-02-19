_asn1_get_octet_string (const unsigned char *der, asn1_node node, int *len)
{
  int len2, len3, counter, tot_len, indefinite;

  counter = 0;

  if (*(der - 1) & ASN1_CLASS_STRUCTURED)
    {
      tot_len = 0;
      indefinite = asn1_get_length_der (der, *len, &len3);
      if (indefinite < -1)
	return ASN1_DER_ERROR;

      counter += len3;
      if (indefinite >= 0)
	indefinite += len3;

      while (1)
	{
	  if (counter > (*len))
	    return ASN1_DER_ERROR;

	  if (indefinite == -1)
	    {
	      if ((der[counter] == 0) && (der[counter + 1] == 0))
		{
		  counter += 2;
		  break;
		}
	    }
	  else if (counter >= indefinite)
	    break;

	  if (der[counter] != ASN1_TAG_OCTET_STRING)
	    return ASN1_DER_ERROR;

	  counter++;

	  len2 = asn1_get_length_der (der + counter, *len - counter, &len3);
	  if (len2 <= 0)
	    return ASN1_DER_ERROR;

	  counter += len3 + len2;
	  tot_len += len2;
	}

      /* copy */
      if (node)
	{
	  unsigned char temp[DER_LEN];
	  int ret;

	  len2 = sizeof (temp);

	  asn1_length_der (tot_len, temp, &len2);
	  _asn1_set_value (node, temp, len2);

	  ret = _asn1_extract_der_octet (node, der, *len);
	  if (ret != ASN1_SUCCESS)
	    return ret;

	}
    }
  else
    {				/* NOT STRUCTURED */
      len2 = asn1_get_length_der (der, *len, &len3);
      if (len2 < 0)
	return ASN1_DER_ERROR;

      counter = len3 + len2;
      if (node)
	_asn1_set_value (node, der, counter);
    }

  *len = counter;
  return ASN1_SUCCESS;

}