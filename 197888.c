addCharacterClass (FileInfo * nested, const widechar * name, int length)
{
/*Define a character class, Whether predefined or user-defined */
  struct CharacterClass *class;
  if (characterClassAttribute)
    {
      if (!(class = malloc (sizeof (*class) + CHARSIZE * (length - 1))))
	outOfMemory ();
      else
	{
	  memset (class, 0, sizeof (*class));
	  memcpy (class->name, name, CHARSIZE * (class->length = length));
	  class->attribute = characterClassAttribute;
	  characterClassAttribute <<= 1;
	  class->next = characterClasses;
	  characterClasses = class;
	  return class;
	}
    }
  compileError (nested, "character class table overflow.");
  return NULL;
}