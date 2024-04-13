getTable (const char *tableList)
{
/*Keep track of which tables have already been compiled */
  int tableListLen;
  ChainEntry *currentEntry = NULL;
  ChainEntry *lastEntry = NULL;
  void *newTable;
  if (tableList == NULL || *tableList == 0)
    return NULL;
  errorCount = fileCount = 0;
  tableListLen = strlen (tableList);
  /*See if this is the last table used. */
  if (lastTrans != NULL)
    if (tableListLen == lastTrans->tableListLength && (memcmp
						       (&lastTrans->
							tableList
							[0],
							tableList,
							tableListLen)) == 0)
      return (table = lastTrans->table);
/*See if Table has already been compiled*/
  currentEntry = tableChain;
  while (currentEntry != NULL)
    {
      if (tableListLen == currentEntry->tableListLength && (memcmp
							    (&currentEntry->
							     tableList
							     [0],
							     tableList,
							     tableListLen))
	  == 0)
	{
	  lastTrans = currentEntry;
	  return (table = currentEntry->table);
	}
      lastEntry = currentEntry;
      currentEntry = currentEntry->next;
    }
  if ((newTable = compileTranslationTable (tableList)))
    {
      /*Add a new entry to the table chain. */
      int entrySize = sizeof (ChainEntry) + tableListLen;
      ChainEntry *newEntry = malloc (entrySize);
      if (!newEntry)
	outOfMemory ();
      if (tableChain == NULL)
	tableChain = newEntry;
      else
	lastEntry->next = newEntry;
      newEntry->next = NULL;
      newEntry->table = newTable;
      newEntry->tableListLength = tableListLen;
      memcpy (&newEntry->tableList[0], tableList, tableListLen);
      lastTrans = newEntry;
      return newEntry->table;
    }
  return NULL;
}