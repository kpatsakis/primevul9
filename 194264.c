_asn1_get_time_der (unsigned type, const unsigned char *der, int der_len, int *ret_len,
		    char *str, int str_size, unsigned flags)
{
  int len_len, str_len;
  unsigned i;
  unsigned sign_count = 0;
  unsigned dot_count = 0;
  const unsigned char *p;

  if (der_len <= 0 || str == NULL)
    return ASN1_DER_ERROR;

  str_len = asn1_get_length_der (der, der_len, &len_len);
  if (str_len <= 0 || str_size < str_len)
    return ASN1_DER_ERROR;

  /* perform some sanity checks on the data */
  if (str_len < 8)
    {
      warn();
      return ASN1_TIME_ENCODING_ERROR;
    }

  if ((flags & ASN1_DECODE_FLAG_STRICT_DER) && !(flags & ASN1_DECODE_FLAG_ALLOW_INCORRECT_TIME))
    {
      p = &der[len_len];
      for (i=0;i<(unsigned)(str_len-1);i++)
         {
           if (isdigit(p[i]) == 0)
             {
               if (type == ASN1_ETYPE_GENERALIZED_TIME)
                 {
                   /* tolerate lax encodings */
                   if (p[i] == '.' && dot_count == 0)
                     {
                       dot_count++;
                       continue;
                     }

               /* This is not really valid DER, but there are
                * structures using that */
                   if (!(flags & ASN1_DECODE_FLAG_STRICT_DER) &&
                       (p[i] == '+' || p[i] == '-') && sign_count == 0)
                     {
                       sign_count++;
                       continue;
                     }
                 }

               warn();
               return ASN1_TIME_ENCODING_ERROR;
             }
         }

      if (sign_count == 0 && p[str_len-1] != 'Z')
        {
          warn();
          return ASN1_TIME_ENCODING_ERROR;
        }
    }
  memcpy (str, der + len_len, str_len);
  str[str_len] = 0;
  *ret_len = str_len + len_len;

  return ASN1_SUCCESS;
}