getCharacters (FileInfo * nested, CharsString * characters)
{
/*Get ruleChars string */
  CharsString token;
  if (getToken (nested, &token, "characters"))
    if (parseChars (nested, characters, &token))
      return 1;
  return 0;
}