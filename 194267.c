asn1_get_length_der (const unsigned char *der, int der_len, int *len)
{
  unsigned int ans;
  int k, punt, sum;

  *len = 0;
  if (der_len <= 0)
    return 0;

  if (!(der[0] & 128))
    {
      /* short form */
      *len = 1;
      ans = der[0];
    }
  else
    {
      /* Long form */
      k = der[0] & 0x7F;
      punt = 1;
      if (k)
	{ /* definite length method */
	  ans = 0;
	  while (punt <= k && punt < der_len)
	    {
	      if (INT_MULTIPLY_OVERFLOW (ans, 256))
		return -2;
	      ans *= 256;

	      if (INT_ADD_OVERFLOW (ans, ((unsigned) der[punt])))
		return -2;
	      ans += der[punt];
	      punt++;
	    }
	}
      else
	{			/* indefinite length method */
	  *len = punt;
	  return -1;
	}

      *len = punt;
    }

  sum = ans;
  if (ans >= INT_MAX || INT_ADD_OVERFLOW (sum, (*len)))
    return -2;
  sum += *len;

  if (sum > der_len)
    return -4;

  return ans;
}