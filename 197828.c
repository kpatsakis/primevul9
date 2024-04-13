compile_findCharOrDots (widechar c, int m)
{
/*Look up a character or dot pattern. If m is 0 look up a character, 
* otherwise look up a dot pattern. Although the algorithms are almost 
* identical, different tables are needed for characters and dots because 
* of the possibility of conflicts.*/
  TranslationTableCharacter *character;
  TranslationTableOffset bucket;
  unsigned long int makeHash = (unsigned long int) c % HASHNUM;
  if (m == 0)
    bucket = table->characters[makeHash];
  else
    bucket = table->dots[makeHash];
  while (bucket)
    {
      character = (TranslationTableCharacter *) & table->ruleArea[bucket];
      if (character->realchar == c)
	return character;
      bucket = character->next;
    }
  return NULL;
}