allocateCharacterClasses ()
{
/*Allocate memory for predifined character classes */
  int k = 0;
  characterClasses = NULL;
  characterClassAttribute = 1;
  while (characterClassNames[k])
    {
      widechar wname[MAXSTRING];
      int length = strlen (characterClassNames[k]);
      int kk;
      for (kk = 0; kk < length; kk++)
	wname[kk] = (widechar) characterClassNames[k][kk];
      if (!addCharacterClass (NULL, wname, length))
	{
	  deallocateCharacterClasses ();
	  return 0;
	}
      k++;
    }
  return 1;
}