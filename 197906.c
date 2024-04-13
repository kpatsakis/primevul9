getRuleDotsText (FileInfo * nested, CharsString * ruleDots)
{
  CharsString token;
  if (getToken (nested, &token, "characters"))
    if (parseChars (nested, ruleDots, &token))
      return 1;
  return 0;
}