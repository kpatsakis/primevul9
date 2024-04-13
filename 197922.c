putCharAndDots (FileInfo * nested, widechar c, widechar d)
{
  TranslationTableOffset bucket;
  CharOrDots *cdPtr;
  CharOrDots *oldcdPtr = NULL;
  TranslationTableOffset offset;
  unsigned long int makeHash;
  if (!(cdPtr = getCharOrDots (c, 0)))
    {
      if (!allocateSpaceInTable (nested, &offset, sizeof (*cdPtr)))
	return 0;
      cdPtr = (CharOrDots *) & table->ruleArea[offset];
      cdPtr->next = 0;
      cdPtr->lookFor = c;
      cdPtr->found = d;
      makeHash = (unsigned long int) c % HASHNUM;
      bucket = table->charToDots[makeHash];
      if (!bucket)
	table->charToDots[makeHash] = offset;
      else
	{
	  oldcdPtr = (CharOrDots *) & table->ruleArea[bucket];
	  while (oldcdPtr->next)
	    oldcdPtr = (CharOrDots *) & table->ruleArea[oldcdPtr->next];
	  oldcdPtr->next = offset;
	}
    }
  if (!(cdPtr = getCharOrDots (d, 1)))
    {
      if (!allocateSpaceInTable (nested, &offset, sizeof (*cdPtr)))
	return 0;
      cdPtr = (CharOrDots *) & table->ruleArea[offset];
      cdPtr->next = 0;
      cdPtr->lookFor = d;
      cdPtr->found = c;
      makeHash = (unsigned long int) d % HASHNUM;
      bucket = table->dotsToChar[makeHash];
      if (!bucket)
	table->dotsToChar[makeHash] = offset;
      else
	{
	  oldcdPtr = (CharOrDots *) & table->ruleArea[bucket];
	  while (oldcdPtr->next)
	    oldcdPtr = (CharOrDots *) & table->ruleArea[oldcdPtr->next];
	  oldcdPtr->next = offset;
	}
    }
  return 1;
}