allocateHeader (FileInfo * nested)
{
/*Allocate memory for the table header and a guess on the number of 
* rules */
  const TranslationTableOffset startSize = 2 * sizeof (*table);
  if (table)
    return 1;
  tableUsed = sizeof (*table) + OFFSETSIZE;	/*So no offset is ever zero */
  if (!(table = malloc (startSize)))
    {
      compileError (nested, "Not enough memory");
      if (table != NULL)
	free (table);
      table = NULL;
      outOfMemory ();
    }
  memset (table, 0, startSize);
  tableSize = startSize;
  return 1;
}