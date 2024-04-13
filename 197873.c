extParseDots (const char *inString, widechar * outString)
{
/* Parse external dot patterns */
  CharsString wideIn;
  CharsString result;
  int k;
  for (k = 0; inString[k] && k < MAXSTRING; k++)
    wideIn.chars[k] = inString[k];
  wideIn.chars[k] = 0;
  wideIn.length = k;
  parseDots (NULL, &result, &wideIn);
  if (errorCount)
    {
      errorCount = 0;
      return 0;
    }
  for (k = 0; k < result.length; k++)
    outString[k] = result.chars[k];
  outString[k] = 0;
  return result.length;
}