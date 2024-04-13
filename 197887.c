compileNumber (FileInfo * nested)
{
  CharsString token;
  widechar dest;
  if (!getToken (nested, &token, "number"))
    return 0;
  getNumber (&token.chars[0], &dest);
  if (!(dest > 0))
    {
      compileError (nested, "a nonzero positive number is required");
      return 0;
    }
  return dest;
}