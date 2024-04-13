getCharacterClass (FileInfo * nested, const struct CharacterClass **class)
{
  CharsString token;
  if (getToken (nested, &token, "character class name"))
    {
      if ((*class = findCharacterClass (&token)))
	return 1;
      compileError (nested, "character class not defined.");
    }
  return 0;
}