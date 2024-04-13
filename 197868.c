deallocateCharacterClasses ()
{
  while (characterClasses)
    {
      struct CharacterClass *class = characterClasses;
      characterClasses = characterClasses->next;
      if (class)
	free (class);
    }
}