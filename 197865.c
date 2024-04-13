passIsKeyword (const char *token)
{
  int k;
  int length = strlen (token);
  int ch = passLine.chars[passLinepos + length + 1];
  if (((ch | 32) >= 'a' && (ch | 32) <= 'z') || (ch >= '0' && ch <= '9'))
    return 0;
  for (k = 0; k < length && passLine.chars[passLinepos + k + 1]
       == (widechar) token[k]; k++);
  if (k == length)
    {
      passLinepos += length + 1;
      return 1;
    }
  return 0;
}