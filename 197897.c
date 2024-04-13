getRuleCharsText (FileInfo * nested, CharsString * ruleChars)
{
  CharsString token;
  if (getToken (nested, &token, "Characters operand"))
    if (parseChars (nested, ruleChars, &token))
      return 1;
  return 0;
}