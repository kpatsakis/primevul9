lou_free ()
{
  ChainEntry *currentEntry;
  ChainEntry *previousEntry;
  if (logFile != NULL)
    fclose (logFile);
  if (tableChain != NULL)
    {
      currentEntry = tableChain;
      while (currentEntry)
	{
	  free (currentEntry->table);
	  previousEntry = currentEntry;
	  currentEntry = currentEntry->next;
	  free (previousEntry);
	}
      tableChain = NULL;
      lastTrans = NULL;
    }
  if (typebuf != NULL)
    free (typebuf);
  typebuf = NULL;
  sizeTypebuf = 0;
  if (destSpacing != NULL)
    free (destSpacing);
  destSpacing = NULL;
  sizeDestSpacing = 0;
  if (passbuf1 != NULL)
    free (passbuf1);
  passbuf1 = NULL;
  sizePassbuf1 = 0;
  if (passbuf2 != NULL)
    free (passbuf2);
  passbuf2 = NULL;
  sizePassbuf2 = 0;
  if (srcMapping != NULL)
    free (srcMapping);
  srcMapping = NULL;
  sizeSrcMapping = 0;
  if (prevSrcMapping != NULL)
    free (prevSrcMapping);
  prevSrcMapping = NULL;
  sizePrevSrcMapping = 0;
  opcodeLengths[0] = 0;
}