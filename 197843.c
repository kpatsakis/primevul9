addCharOrDots (FileInfo * nested, widechar c, int m)
{
/*See if a character or dot pattern is in the appropriate table. If not, 
* insert it. In either 
* case, return a pointer to it. */
  TranslationTableOffset bucket;
  TranslationTableCharacter *character;
  TranslationTableCharacter *oldchar;
  TranslationTableOffset offset;
  unsigned long int makeHash;
  if ((character = compile_findCharOrDots (c, m)))
    return character;
  if (!allocateSpaceInTable (nested, &offset, sizeof (*character)))
    return NULL;
  character = (TranslationTableCharacter *) & table->ruleArea[offset];
  memset (character, 0, sizeof (*character));
  character->realchar = c;
  makeHash = (unsigned long int) c % HASHNUM;
  if (m == 0)
    bucket = table->characters[makeHash];
  else
    bucket = table->dots[makeHash];
  if (!bucket)
    {
      if (m == 0)
	table->characters[makeHash] = offset;
      else
	table->dots[makeHash] = offset;
    }
  else
    {
      oldchar = (TranslationTableCharacter *) & table->ruleArea[bucket];
      while (oldchar->next)
	oldchar =
	  (TranslationTableCharacter *) & table->ruleArea[oldchar->next];
      oldchar->next = offset;
    }
  return character;
}