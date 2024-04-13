hexValue (FileInfo * nested, const widechar * digits, int length)
{
  int k;
  unsigned int binaryValue = 0;
  for (k = 0; k < length; k++)
    {
      unsigned int hexDigit = 0;
      if (digits[k] >= '0' && digits[k] <= '9')
	hexDigit = digits[k] - '0';
      else if (digits[k] >= 'a' && digits[k] <= 'f')
	hexDigit = digits[k] - 'a' + 10;
      else if (digits[k] >= 'A' && digits[k] <= 'F')
	hexDigit = digits[k] - 'A' + 10;
      else
	{
	  compileError (nested, "invalid %d-digit hexadecimal number",
			length);
	  return (widechar) 0xffffffff;
	}
      binaryValue |= hexDigit << (4 * (length - 1 - k));
    }
  return (widechar) binaryValue;
}