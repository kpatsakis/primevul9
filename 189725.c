_asn1_ltostr (int64_t v, char str[LTOSTR_MAX_SIZE])
{
  uint64_t d, r;
  char temp[LTOSTR_MAX_SIZE];
  int count, k, start;
  uint64_t val;

  if (v < 0)
    {
      str[0] = '-';
      start = 1;
      val = -((uint64_t)v);
    }
  else
    {
      val = v;
      start = 0;
    }

  count = 0;
  do
    {
      d = val / 10;
      r = val - d * 10;
      temp[start + count] = '0' + (char) r;
      count++;
      val = d;
    }
  while (val && ((start+count) < LTOSTR_MAX_SIZE-1));

  for (k = 0; k < count; k++)
    str[k + start] = temp[start + count - k - 1];
  str[count + start] = 0;
  return str;
}