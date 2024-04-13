definedCharOrDots (FileInfo * nested, widechar c, int m)
{
  TranslationTableCharacter *notFound;
  TranslationTableCharacter *charOrDots = compile_findCharOrDots (c, m);
  if (charOrDots)
    return charOrDots;
  if (m == 0)
    {
      notFound = &noChar;
      compileError (nested,
		    "character %s should be defined at this point but is not",
		    showString (&c, 1));
    }
  else
    {
      notFound = &noDots;
      compileError (nested,
		    "cell %s should be defined at this point but is not",
		    unknownDots (c));
    }
  return notFound;
}